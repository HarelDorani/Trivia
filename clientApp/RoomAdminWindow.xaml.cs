using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace clientApp
{
    public partial class RoomAdminWindow : Window
    {
        private RoomInfo _roomInfo;  //info of the room
        private List<string> _players;  //players in the room
        private ClientCommunicator _comm;
        private bool _isAdmin;  //is curr player admin or not
        // Used to cansle tasks
        private CancellationTokenSource _cts = new CancellationTokenSource();
        // Used specifically to cancel the room state polling task
        private CancellationTokenSource _roomStateCts = new CancellationTokenSource();
        // Task that repeatedly checks the room state from the server
        private Task _roomStateTask;
        // Lock object for safely updating/accessing the player list from multiple threads
        private readonly object _playerListLock = new object();
        private int _questionCount;  //num of questions in the room
        private float _timePerQuestion;  //time per question


        public RoomAdminWindow(RoomInfo roomInfo, List<string> players, ClientCommunicator comm, bool isAdmin)
        {
            InitializeComponent();

            _roomInfo = roomInfo;  //setting room info
            _players = new List<string>(players);  //setting the players in the room
            _comm = comm;  //setting the communicator
            _isAdmin = isAdmin;  //setting if player is admin
            _questionCount = _roomInfo.numOfQuestionsInGame;  //setting the num of questions
            _timePerQuestion = _roomInfo.timePerQuestion;  //setting the time per question

            this.Title = $"Room - {_roomInfo.name}";  //showing room name on the screen
            RoomNameTextBlock.Text = _roomInfo.name;
            PlayersListBox.ItemsSource = _players;  //adding players to the screen

            //showing start game button only if player is admin
            StartGameButton.Visibility = _isAdmin ? Visibility.Visible : Visibility.Collapsed;
            //showing close room button only if player is admin
            CloseRoomButton.Visibility = _isAdmin ? Visibility.Visible : Visibility.Collapsed;
            //showing leave room button only if player is not admin
            LeaveRoomButton.Visibility = !_isAdmin ? Visibility.Visible : Visibility.Collapsed;

            if (!_isAdmin)
            {
                //if player is not the admin calling the func that listens for responses from the server
                StartListeningForServerEvents();
            }
            StartRoomStatePolling();  //calling func that updates the players list every 3 secs
        }

        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // building start game request with the room id and code 11
                var startReq = new { roomId = _roomInfo.id };
                byte[] buffer = CommunicationHelper.BuildMessage(11, startReq);
                _comm.Send(buffer);  //sending request to the server

                byte[] response = _comm.Receive();  //getting the server response
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 11)  //response code match the request code
                {
                    //converting the json
                    dynamic startRes = JsonConvert.DeserializeObject(json);
                    if (((string)startRes.status).ToUpper() == "SUCCESS")  //is status was success
                    {
                        //canceling background tasks
                        _cts.Cancel();
                        _roomStateCts.Cancel();
                        //creating new game screen window with question count and time per question 
                        new gameScreen(_questionCount, (int)_timePerQuestion, _comm).Show();
                        this.Close();  //closing curr window
                    }
                    else
                    {
                        MessageBox.Show("Failed to start game.", "Error");
                    }
                }
                else if (code == 100)  //error code
                {
                    dynamic errRes = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Network error: {ex.Message}", "Error");
            }
        }



        private void LeaveRoomButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _cts.Cancel();           // canceling general background operations
                _roomStateCts.Cancel();  // canceling the room state polling task

                //building new leave room request with the room id and code 13
                var request = new { roomId = _roomInfo.id };
                byte[] buffer = CommunicationHelper.BuildMessage(13, request);
                _comm.Send(buffer);  //sending request to the server

                byte[] response = _comm.Receive();  //getting the server response
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 13)  //if response code match the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (((string)res.status).ToUpper() == "SUCCESS") //checking if status is success
                    {
                        //creating new menu window and closing curr window
                        new MenuWindow(_comm).Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Failed to leave room.", "Error");
                    }
                }
                else if (code == 100)  //error code
                {
                    dynamic errRes = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Leave request failed: {ex.Message}", "Error");
            }
        }

        private void CloseRoomButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //building close room request with the room id and code 10
                var request = new { roomId = _roomInfo.id };
                byte[] buffer = CommunicationHelper.BuildMessage(10, request);
                _comm.Send(buffer);  //sending request to the server

                byte[] response = _comm.Receive();  //getting the server response
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 10)  //if response code match the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (((string)res.status).ToUpper() == "SUCCESS") //checking if status is success
                    {
                        //canceling background tasks
                        _cts.Cancel();
                        _roomStateCts.Cancel();
                        //creating a new menu window and closing curr window
                        new MenuWindow(_comm).Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Failed to close room.", "Error");
                    }
                }
                else if (code == 100)  //error code
                {
                    dynamic errRes = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Network error: {ex.Message}", "Error");
            }
        }


        //this function creates a new thread that listen for server responses 
        private void StartListeningForServerEvents()
        {
            Task.Run(() =>
            {
                try
                {
                    //loop as long the task wasnt cansled
                    while (!_cts.Token.IsCancellationRequested)
                    {
                        try
                        {
                            //getting response from the server
                            byte[] response = _comm.Receive();
                            //parsing response
                            var (code, json) = CommunicationHelper.ParseMessage(response);

                            //checking if the code is 1 of this responses:
                            switch (code)
                            {
                                case 11:  //response for start game
                                    HandleStartGame(json);  //calling the func that handles start game response
                                    return;

                                case 13:  //response for leave room
                                    HandleLeaveRoom(json);  //calling the func that handle leave room response
                                    return;
                            }
                        }
                        catch (TimeoutException)
                        {
                        }
                        catch (Exception ex)
                        {
                            Dispatcher.Invoke(() =>
                            {
                                MessageBox.Show($"Disconnected: {ex.Message}", "Error");
                                new MenuWindow(_comm).Show();
                                this.Close();
                            });
                            return;
                        }
                    }
                }
                catch (Exception ex)
                {
                    Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show($"Disconnected: {ex.Message}", "Error");
                        new MenuWindow(_comm).Show();
                        this.Close();
                    });
                }
            }, _cts.Token);
        }


        //this function creates a new thread that request for room state every 3 secs to update the players list
        private void StartRoomStatePolling()
        {
            //creating new thread
            _roomStateTask = Task.Run(async () =>
            {
                //loop as lonse the task wasnt cancled
                while (!_roomStateCts.Token.IsCancellationRequested)
                {
                    try
                    {
                        //building new get room state request with the room id and code 12
                        var request = new { roomId = _roomInfo.id };
                        byte[] buffer = CommunicationHelper.BuildMessage(12, request);
                        _comm.Send(buffer);  //sending request to the server

                        //getting the server response
                        byte[] response = _comm.Receive();
                        //parsing the response
                        var (code, json) = CommunicationHelper.ParseMessage(response);

                        if (code == 12)  //checking if response code match the request code
                        {
                            //converting the json
                            dynamic res = JsonConvert.DeserializeObject(json);
                            if (((string)res.status).ToUpper() == "SUCCESS")  //checking if status was success
                            {
                                //creating new list of players
                                List<string> newPlayers = new List<string>();
                                foreach (var p in res.players)  //loop through all players from the response
                                {
                                    //adding each player from the response to the list
                                    newPlayers.Add((string)p);
                                }

                                Dispatcher.Invoke(() =>
                                {
                                    lock (_playerListLock)  //locking before updating the players list
                                    {
                                        //updating the players list and updating the screen to show the new list
                                        _players = newPlayers;
                                        PlayersListBox.ItemsSource = null;
                                        PlayersListBox.ItemsSource = _players;
                                    }
                                });
                            }
                        }
                        else if (code == 100)  //error code
                        {
                            dynamic errRes = JsonConvert.DeserializeObject(json);
                            MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                    }
                    catch (Exception)
                    {
                    }

                    try
                    {
                        //making the task wait 3 seconds before runnnig again
                        await Task.Delay(3000, _roomStateCts.Token);
                    }
                    catch (TaskCanceledException)
                    {
                        break;
                    }
                }
            }, _roomStateCts.Token);
        }


        //this function handle the start game response from the server
        private void HandleStartGame(string json)
        {
            //converting the json
            dynamic res = JsonConvert.DeserializeObject(json);
            if (((string)res.status).ToUpper() == "SUCCESS")  //if status is success
            {
                //canceling all background tasks
                _cts.Cancel();
                _roomStateCts.Cancel();
                Dispatcher.Invoke(() =>
                {
                    //creating new game screen window with the question count and time per question
                    new gameScreen(_questionCount, (int)_timePerQuestion, _comm).Show();
                    this.Close();  //closing curr window
                });
            }
        }

        //this function handle leave room response from the server
        private void HandleLeaveRoom(string json)
        {
            //converting the json
            dynamic res = JsonConvert.DeserializeObject(json);
            Dispatcher.Invoke(() =>
            {
                if (((string)res.status).ToUpper() == "SUCCESS")  //checking if status is success
                {
                    MessageBox.Show("You have left the room.", "Info");
                    //canceling background tasks
                    _cts.Cancel();
                    _roomStateCts.Cancel();
                    //creating new menu window and closing curr window
                    new MenuWindow(_comm).Show();
                    this.Close();
                }
                else
                {
                    MessageBox.Show("Failed to leave the room.", "Error");
                }
            });
        }


        //function to cancle all background tasks when curr window is closes
        protected override void OnClosed(EventArgs e)
        {
            _cts.Cancel();
            _roomStateCts.Cancel();
            base.OnClosed(e);
        }
    }
}

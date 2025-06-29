using Newtonsoft.Json;
using System;
using System.Collections.ObjectModel;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace clientApp
{
    //window of all rooms available
    public partial class RoomsWindow : Window
    {
        private readonly ClientCommunicator _comm;
        private List<RoomInfo> _rooms;  //the list of rooms
        private readonly object _commLock = new object();   //lock opjects when _comm used
        //used for canceling the background tasks
        private CancellationTokenSource _cts = new CancellationTokenSource();
        // Task that repeatedly checks the list of rooms from the server
        private Task _pollingTask;
        public RoomsWindow(List<RoomInfo> rooms, ClientCommunicator communicator)
        {
            InitializeComponent();
            _comm = communicator;  //setting communicator
            _rooms = new List<RoomInfo>(rooms);  //setting the rooms list
            RoomsListBox.ItemsSource = _rooms;  //showing roon list on the screen
            StartRoomPolling();  //calling func that updates the rooms list every 3 secs
        }

        private void RoomsListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (RoomsListBox.SelectedItem is RoomInfo selectedRoom)
            {
                try
                {
                    lock (_commLock)
                    {
                        //building request of join room with the selcted room's id
                        var joinRoomRequest = new { roomId = selectedRoom.id };
                        //sending request to the server
                        _comm.Send(CommunicationHelper.BuildMessage(6, joinRoomRequest));
                        //parsing the response from the server
                        var (joinCode, joinJson) = CommunicationHelper.ParseMessage(_comm.Receive());
                        if (joinCode == 6) //checking if response code match the request code
                        {
                            //converting the json
                            dynamic joinRes = JsonConvert.DeserializeObject(joinJson);
                            if (((string)joinRes.status).ToUpper() == "SUCCESS")  //checking if status is success
                            {
                                //creating new request of room state with the selected room id and code of 12
                                var getRoomStateRequest = new { roomId = selectedRoom.id };
                                //sending request to the server
                                _comm.Send(CommunicationHelper.BuildMessage(12, getRoomStateRequest));
                                //getting the response from the server  and parsing it
                                var (stateCode, stateJson) = CommunicationHelper.ParseMessage(_comm.Receive());
                                if (stateCode == 12)  //checking if response code match the request code
                                {
                                    //converting the json string
                                    dynamic stateRes = JsonConvert.DeserializeObject(stateJson);
                                    if (((string)stateRes.status).ToUpper() == "SUCCESS")  //checking if stauts response was success
                                    {
                                        //converting the players list from the response into new list
                                        var players = JsonConvert.DeserializeObject<List<string>>(Convert.ToString(stateRes.players));
                                        //canceling background tasks
                                        _cts.Cancel();
                                        _pollingTask?.Wait(1000);
                                        //creating new room admin window with the selcted room info, the players list, and admin as false because didnt create the room
                                        new RoomAdminWindow(selectedRoom, players, _comm, false).Show();
                                        this.Close(); //closing curr room
                                    }
                                    else MessageBox.Show("Failed to get room state.", "Error");
                                }
                                else if (stateCode == 100)  //error code
                                {
                                    dynamic errRes = JsonConvert.DeserializeObject(stateJson);
                                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                                }
                                else MessageBox.Show("Unexpected room state response.", "Protocol Error");
                            }
                            else MessageBox.Show("Failed to join room.", "Join Error");
                        }
                        else if (joinCode == 100)  //error code
                        {
                            dynamic errRes = JsonConvert.DeserializeObject(joinJson);
                            MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                        else MessageBox.Show("Unexpected join response.", "Protocol Error");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Network error: {ex.Message}", "Error");
                }
            }
        }


        //this function creates a new thread that request the server for all available rooms every 3 secs and update the rooms list
        private void StartRoomPolling()
        {
            //creating new thread
            _pollingTask = Task.Run(async () =>
            {
                //loop as long task wasnt cansled
                while (!_cts.Token.IsCancellationRequested)
                {
                    try
                    {
                        lock (_commLock)
                        {
                            //creating get rooms request with code 4
                            var request = new {};
                            //building the buffer
                            byte[] buffer = CommunicationHelper.BuildMessage(4, request);
                            _comm.Send(buffer);  //sending request to the server
                            byte[] response = _comm.Receive();  //getting the server response
                            //parsing the response
                            var (code, json) = CommunicationHelper.ParseMessage(response);
                            if (code == 4)  //checking if response code match the request code
                            {
                                //converting the json
                                dynamic res = JsonConvert.DeserializeObject(json);
                                if (((string)res.status).ToUpper() == "SUCCESS") //checking if response status is success
                                {
                                    if (res.results != null)
                                    {
                                        //deserializing the results of rooms from the response into a list of room info
                                        List<RoomInfo> newRooms = JsonConvert.DeserializeObject<List<RoomInfo>>(Convert.ToString(res.results));

                                        Dispatcher.Invoke(() =>
                                        {
                                            //updating the rooms list to be the new list
                                            _rooms = newRooms;
                                            RoomsListBox.ItemsSource = null;
                                            RoomsListBox.ItemsSource = _rooms;  //showing new list on the screen
                                        });
                                    }
                                }
                            }
                            else if (code == 100)  //error code
                            {
                                dynamic errRes = JsonConvert.DeserializeObject(json);
                                MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                            }
                        }
                    }
                    catch (Exception)
                    {
                    }

                    try
                    {
                        //waiting 3 secs before running again
                        await Task.Delay(3000, _cts.Token);
                    }
                    catch (TaskCanceledException)
                    {
                        break;
                    }
                }
            }, _cts.Token);
        }

        //function to cancle all background tasks when curr window is closes
        protected override async void OnClosed(EventArgs e)
        {
            _cts.Cancel();
            if (_pollingTask != null)
                await _pollingTask;
            base.OnClosed(e);
        }
    }
}

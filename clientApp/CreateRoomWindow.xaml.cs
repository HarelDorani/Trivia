using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
namespace clientApp
{
    /// <summary>
    /// Interaction logic for CreateRoomWindow.xaml
    /// </summary>
    public partial class CreateRoomWindow : Window
    {
        private ClientCommunicator _comm;
        public CreateRoomWindow(ClientCommunicator comm)
        {
            InitializeComponent();
            _comm = comm;  //setting the communicator
        }

        private void CreateButton_Click(object sender, RoutedEventArgs e)
        {
            //converting all input to text
            string roomName = RoomNameTextBox.Text.Trim();
            string timeStr = TimePerQuestionTextBox.Text.Trim();
            string playersStr = NumberOfPlayersTextBox.Text.Trim();
            string questionsStr = NumberOfQuestionsTextBox.Text.Trim();

            //checking if got input for all the fields
            if (string.IsNullOrEmpty(roomName))
            {
                MessageBox.Show("Please enter a room name.", "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (!int.TryParse(timeStr, out int timePerQuestion) || timePerQuestion <= 0)
            {
                MessageBox.Show("Please enter a valid positive number for time per question.", "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (!int.TryParse(playersStr, out int numberOfPlayers) || numberOfPlayers <= 0)
            {
                MessageBox.Show("Please enter a valid positive number for number of players.", "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (!int.TryParse(questionsStr, out int numberOfQuestions) || numberOfQuestions <= 0)
            {
                MessageBox.Show("Please enter a valid positive number for number of questions.", "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            //creating a new createRoomRequest with all the input fields
            var createRoomRequest = new
            {
                roomName = roomName,
                maxUsers = numberOfPlayers,
                questionCount = numberOfQuestions,
                answerTimeout = timePerQuestion
            };

            try
            {
                //building the msg
                byte[] createBuffer = CommunicationHelper.BuildMessage(7, createRoomRequest);
                _comm.Send(createBuffer);  //sending the msg to the server

                byte[] createResponse = _comm.Receive();  //receiving response from the server
                //parsing the response
                var (createCode, createJson) = CommunicationHelper.ParseMessage(createResponse);

                // 7 = create room response code
                if (createCode == 7)
                {
                    //converting the json
                    dynamic createRes = JsonConvert.DeserializeObject(createJson);
                    if (((string)createRes.status).ToUpper() == "SUCCESS")  //if status is success
                    {
                        int roomId = (int)createRes.roomID;  ///getting the room id

                        //sending get room state request
                        var roomStateRequest = new { roomId = roomId };  //requesting room state of the curr room
                        //building the msg with code 12 - get room state request code
                        byte[] stateBuffer = CommunicationHelper.BuildMessage(12, roomStateRequest);
                        _comm.Send(stateBuffer);  //sending the msg to the server

                        byte[] stateResponse = _comm.Receive();  //receiving response from the server
                        //parsing the msg
                        var (stateCode, stateJson) = CommunicationHelper.ParseMessage(stateResponse);

                        if (stateCode == 12)  //if response is ok for get room state req witch is 12
                        {
                            //converting the json
                            dynamic stateRes = JsonConvert.DeserializeObject(stateJson);
                            if (((string)stateRes.status).ToUpper() == "SUCCESS")  //checking if status is success
                            {
                                //list of players
                                List<string> players = new List<string>();
                                foreach (var p in stateRes.players)
                                {
                                    players.Add((string)p);  //add each player from the response to the list
                                }
                                //creating a new roomInfo instance with the input data and data from server
                                var roomInfo = new RoomInfo
                                {
                                    id = roomId,
                                    maxPlayers = numberOfPlayers,
                                    name = roomName,
                                    numOfQuestionsInGame = numberOfQuestions,
                                    state = false,
                                    timePerQuestion = timePerQuestion
                                };

                                //creating a new room admin window with the room info, the players, the comm instance, and setting admin to true
                                var adminWindow = new RoomAdminWindow(roomInfo, players, _comm, true);
                                adminWindow.Show();
                                this.Close();  //closing curr window
                                return;
                            }
                            else
                            {
                                MessageBox.Show("Room created, but failed to get room state.", "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                            }
                        }
                        else if (stateCode == 100)
                        {
                            dynamic res = JsonConvert.DeserializeObject(stateJson);
                            MessageBox.Show($"Error: {res.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                        else
                        {
                            MessageBox.Show("Room created, but server returned invalid room state response.", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                        }
                    }
                    else
                    {
                        MessageBox.Show("Room creation failed.", "Server Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                    }
                }
                else
                {
                    MessageBox.Show("Unexpected response to create room.", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }



        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            //if cancle button clicked returning to menu window
            var menuWindow = new MenuWindow(_comm);
            menuWindow.Show();
            this.Close();
        }

    }
}

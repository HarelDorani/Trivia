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
    /// Interaction logic for MenuWindow.xaml
    /// </summary>
    public partial class MenuWindow : Window
    {
        private ClientCommunicator _comm;
        public MenuWindow(ClientCommunicator comm)
        {
            InitializeComponent();
            _comm = comm;  //setting the communicator
        }

        private void CreateRoom_Click(object sender, RoutedEventArgs e)
        {
            //creating new window of create room and closing curr window
            var createRoomWindow = new CreateRoomWindow(_comm);
            createRoomWindow.Show();
            this.Close();
        }
        private void LogoutButton_Click(object sender, RoutedEventArgs e)
        {
            var logoutRequest = new
            {
            };

            try
            {
                //building msg for logout request with code 3
                byte[] buffer = CommunicationHelper.BuildMessage(3, logoutRequest);
                _comm.Send(buffer); //sending request to the server

                byte[] response = _comm.Receive();  //getting response from the server
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 3) // response code match request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (res.status == "SUCCESS")  //if status is success
                    {
                        //loading new logout window and closing curr window
                        LoginWindow loginWindow = new LoginWindow(_comm);
                        loginWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Logout failed.", "Logout Failed", MessageBoxButton.OK, MessageBoxImage.Warning);
                    }
                }
                else if (code == 100) // Error response code
                {
                    dynamic res = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {res.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected response from server.", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Logout failed: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
        private void JoinRoom_Click(object sender, RoutedEventArgs e)
        {
            var getRoomsRequest = new
            {
            };

            try
            {
                //building request of get rooms with code 4
                byte[] buffer = CommunicationHelper.BuildMessage(4, getRoomsRequest);
                _comm.Send(buffer); //sending request to the server

                byte[] response = _comm.Receive();  //getting the server response
                var (code, json) = CommunicationHelper.ParseMessage(response); //parsing the response

                if (code == 4)  //response code match request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (res.status == "SUCCESS")  //satus was success
                    {
                        // converting the results from the response into list of roomInfo elements
                        List<RoomInfo> rooms = JsonConvert.DeserializeObject<List<RoomInfo>>(Convert.ToString(res.results));
                        
                        //creating new window of rooms and closing curr window
                        RoomsWindow roomsWindow = new RoomsWindow(rooms, _comm); 
                        roomsWindow.Show();
                        this.Close();
                        
                    }
                }
                else if (code == 100)
                {
                    dynamic res = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {res.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected response from server.", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to get rooms: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }


        private void PersonalStats_Click(object sender, RoutedEventArgs e)
        {
            var getPersonalStatsRequest = new
            {
            };

            try
            {
                //building request of get personal stats with code 9
                byte[] buffer = CommunicationHelper.BuildMessage(9, getPersonalStatsRequest);
                _comm.Send(buffer);  //sending the request

                byte[] response = _comm.Receive();  //getting server response
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 9)  //response code match the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (res.status == "SUCCESS")  //status was success
                    {
                        // converting the personal stats vector from the response to list of strings
                        List<string> personalStats = JsonConvert.DeserializeObject<List<string>>(Convert.ToString(res.PersonalStats));

                        if (personalStats.Count == 0)
                        {
                            MessageBox.Show("No personal statistics available.", "Personal Stats", MessageBoxButton.OK, MessageBoxImage.Information);
                            return;
                        }
                        else
                        {
                            //loading the personal stats window with the stats list and closing curr window
                            PersonalStatsWindow statsWindow = new PersonalStatsWindow(personalStats, _comm);
                            statsWindow.Show();
                            this.Close();
                        }
                    }
                    else
                    {
                        MessageBox.Show("Failed to retrieve personal statistics.", "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                    }
                }
                else if (code == 100)  //error msg code
                {
                    dynamic res = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {res.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected response from server.", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to get personal statistics: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

        }

        private void HighScores_Click(object sender, RoutedEventArgs e)
        {
            var getHighScoreRequest = new
            {
            };

            try
            {
                //building request of high scores with code 8
                byte[] buffer = CommunicationHelper.BuildMessage(8, getHighScoreRequest);
                _comm.Send(buffer);    //sending the request

                byte[] response = _comm.Receive();  //getting response from the server
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 8)  //if response code match the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (res.status == "SUCCESS")   //status was success
                    {
                        // Deserialize the "HighScore" array as List<string>
                        List<string> highScores = JsonConvert.DeserializeObject<List<string>>(Convert.ToString(res.HighScore));

                        if (highScores.Count == 0)
                        {
                            MessageBox.Show("No high scores available.", "High Scores", MessageBoxButton.OK, MessageBoxImage.Information);
                            return;
                        }
                        else
                        {
                            //opening new high score window with the high scores list and closing curr window
                            highScoreWindow highScoreWindow = new highScoreWindow(highScores, _comm);
                            highScoreWindow.Show();
                            this.Close();
                        }
                    }
                    else
                    {
                        MessageBox.Show("Failed to retrieve high scores.", "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                    }
                }
                else if (code == 100)
                {
                    dynamic res = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {res.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected response from server.", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to get high scores: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}

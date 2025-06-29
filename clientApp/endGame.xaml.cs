using Newtonsoft.Json;
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
    /// Interaction logic for endGame.xaml
    /// </summary>
    
    //class that hold players result info
    public class PlayerResult
    {
        [JsonProperty("username")]
        public string Username { get; set; }

        [JsonProperty("correctAnswerCount")]
        public int CorrectAnswers { get; set; }

        [JsonProperty("averageAnswerTime")]
        public double AverageTime { get; set; }
    }

    public partial class endGame : Window
    {
        private readonly ClientCommunicator _comm;
        public endGame(ClientCommunicator communicator)
        {
            InitializeComponent();
            _comm = communicator;  //setting the communicator
            //calling func that loads the results of the game
            LoadResults();
        }
        
        //this func loads the results of the game for each player in the game
        private void LoadResults()
        {
            try
            {
                var request = new { };
                //building msg of getGameResults request with the code of request - 15
                byte[] buffer = CommunicationHelper.BuildMessage(15, request);
                _comm.Send(buffer);   //sending to the server
                //receiving the server response
                byte[] response = _comm.Receive();
                //parsing the msg
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 15)  //checking if response code feets the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (((string)res.status).ToUpper() == "SUCCESS")  //checking if status is success
                    {
                        var resultsJson = res.results.ToString();
                        //converting the json to list of players results, server returns the results sorted from higher to lower
                        List<PlayerResult> results = JsonConvert.DeserializeObject<List<PlayerResult>>(resultsJson);

                        ResultsGrid.ItemsSource = results;  //adding the results to the screen

                        if (results.Count > 0)  
                        {
                            //first player in the list is the winner
                            WinnerText.Text = $"🏆 Winner: {results[0].Username}";
                        }
                    }
                    else
                    {
                        MessageBox.Show("Failed to get game results.", "Error");
                    }
                }
                else if (code == 100)
                {
                    dynamic res = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {res.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected server response.", "Error");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Loading results failed: {ex.Message}", "Error");
            }
        }

    }
}

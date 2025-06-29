using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Threading;

namespace clientApp
{
    //window of the game screen
    public partial class gameScreen : Window
    {
        private readonly ClientCommunicator _comm;
        private readonly int _totalQuestions;
        private readonly int _timePerQuestion;

        private int _currentQuestionIndex = 0;
        private int _correctAnswers = 0;
        private int _timeLeft;
        private DispatcherTimer _timer;

        private string _currentQuestion;
        private List<string> _answers;
        private int _correctAnswerID;

        public gameScreen(int questionCount, int timePerQuestion, ClientCommunicator communicator)
        {
            InitializeComponent();

            _totalQuestions = questionCount;  //setting the num of questions in the game
            _timePerQuestion = timePerQuestion;  //setting the time per question
            _comm = communicator;  //setting the communicator

            LoadNextQuestion();  //calling func that loads the next question for the user
        }


        //this func loads the next question in the game
        private void LoadNextQuestion()
        {
            //checking if curr question is last question
            if (_currentQuestionIndex >= _totalQuestions)
            {
                EndGame();  //calling func that handle the end of the  game
                return;
            }

            try
            {
                var request = new { };
                //building the request of getting next question with code 16
                byte[] buffer = CommunicationHelper.BuildMessage(16, request);
                _comm.Send(buffer);  //sending to the server

                byte[] response = _comm.Receive();  //getting the response
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 16)  //if response code feets request code
                {
                    //parsing the json msg from the server
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (((string)res.status).ToUpper() == "SUCCESS")  //if status is success
                    {
                        _currentQuestion = res.question;  //getting the curr question from the response
                        //getting the possible answers from the response
                        _answers = ((JArray)res.answers).ToObject<List<string>>();
                        //getting the id of correct answer from the response
                        _correctAnswerID = (int)res.correctAnswerId;

                        // updating the UI screen
                        QuestionText.Text = _currentQuestion;
                        AnswerButton1.Content = _answers[0];
                        AnswerButton2.Content = _answers[1];
                        AnswerButton3.Content = _answers[2];
                        AnswerButton4.Content = _answers[3];

                        RemainingQuestionsText.Text = (_totalQuestions - _currentQuestionIndex).ToString();
                        CorrectAnswersText.Text = _correctAnswers.ToString();

                        EnableAnswerButtons(true);  //enable to submit answer
                        StartTimer();  //starting coundown
                    }
                    else
                    {
                        MessageBox.Show("Failed to get question.", "Error");
                    }
                }
                else if (code == 100)
                {
                    dynamic errRes = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Get question request failed: {ex.Message}", "Error");
            }
        }

        // starts the countdown timer for the current question
        private void StartTimer()
        {
            _timeLeft = _timePerQuestion;  //getting the time per question
            TimerText.Text = _timeLeft.ToString();  //putting time on screen

            if (_timer == null)  //creating timer if doesnt exist
            {
                _timer = new DispatcherTimer
                {
                    Interval = TimeSpan.FromSeconds(1) // setting timer to tick every second
                };
                _timer.Tick += Timer_Tick;  // Assign tick event handler
            }
            _timer.Start();  //starting the timer
        }

        // Called every second by the timer to update countdown
        private void Timer_Tick(object sender, EventArgs e)
        {
            _timeLeft--;  //decreasing timer by 1 sec each time
            TimerText.Text = _timeLeft.ToString();  //updating the time in the screen

            if (_timeLeft <= 0)  //if times up
            {
                _timer.Stop();  //stoping the timer
            }
        }


        //function that handler an answer button click
        private void AnswerButton_Click(object sender, RoutedEventArgs e)
        {
            _timer.Stop();    //stopping the timer

            int clickedIndex = -1;
            //getting index of clicked answer
            if (sender == AnswerButton1) clickedIndex = 0;
            else if (sender == AnswerButton2) clickedIndex = 1;
            else if (sender == AnswerButton3) clickedIndex = 2;
            else if (sender == AnswerButton4) clickedIndex = 3;

            SubmitAnswer(clickedIndex);  //calling func that submits the answer with the answer index
        }


        //this function submits answer to the server
        private void SubmitAnswer(int selectedAnswer)
        {
            EnableAnswerButtons(false);  //not alowing to submit more answers

            try
            {
                //creating new request with the id of selected answer
                var request = new { answerId = selectedAnswer };
                //sending msg with code 14 of submit answer request
                byte[] buffer = CommunicationHelper.BuildMessage(14, request);
                _comm.Send(buffer);  //sending msg to the server

                byte[] response = _comm.Receive();  //getting response from the server
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 14)  //if response code feets the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (((string)res.status).ToUpper() == "SUCCESS")  // checking if status is success
                    {
                        //getting the correct answer id from the server
                        int correctAnswerId = (int)res.correctAnswerId;
                        //if user chose the correct answer id increasing the correct answers var
                        if (selectedAnswer == correctAnswerId)
                            _correctAnswers++;
                    }
                    else
                    {
                        MessageBox.Show("Failed to submit answer.", "Error");
                    }
                }
                else if (code == 100)
                {
                    dynamic errRes = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected server response.", "Error");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Submit answer failed: {ex.Message}", "Error");
            }

            _currentQuestionIndex++;  //moving index of curr question to be the next
            LoadNextQuestion();  //calling func that loads the next question
        }


        //this function enable the buttons of answers by bool input
        private void EnableAnswerButtons(bool enabled)
        {
            AnswerButton1.IsEnabled = enabled;
            AnswerButton2.IsEnabled = enabled;
            AnswerButton3.IsEnabled = enabled;
            AnswerButton4.IsEnabled = enabled;
        }


        //function for end game button
        private void EndGameButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var request = new { };
                //building new msg request with code 17 of leave game request
                byte[] buffer = CommunicationHelper.BuildMessage(17, request);
                _comm.Send(buffer); //sending req to the server

                byte[] response = _comm.Receive();  //getting response from the server
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 17)  // checking if response code match the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (((string)res.status).ToUpper() == "SUCCESS") //checking if status is success
                    {
                        //opening the menu window and closing curr window
                        var menuWindow = new MenuWindow(_comm);
                        menuWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Failed to leave game: ", "Error");
                    }
                }
                else if (code == 100)
                {
                    dynamic errRes = JsonConvert.DeserializeObject(json);
                    MessageBox.Show($"Error: {errRes.message}", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show("Unexpected server response.", "Error");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"leave game failed: {ex.Message}", "Error");
            }
        }

        //this func handle the end of the game
        private void EndGame()
        {
            _timer?.Stop();  //stopping timer
            //opening new window of end game and closing curr
            new endGame(_comm).Show();
            this.Close();
        }
    }
}

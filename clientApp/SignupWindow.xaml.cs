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
    /// Interaction logic for SignupWindow.xaml
    /// </summary>
    public partial class SignupWindow : Window
    {
        private ClientCommunicator _comm;
        public SignupWindow(ClientCommunicator comm)
        {
            InitializeComponent();
            _comm = comm;  //setting the communicator
        }
        private void SignupButton_Click(object sender, RoutedEventArgs e)
        {
            //getting all input
            string username = UsernameTextBox.Text;
            string password = PasswordBox.Password;
            string email = EmailTextBox.Text;

            //checking if inputs are empty
            if (string.IsNullOrWhiteSpace(username) ||
                string.IsNullOrWhiteSpace(password) ||
                string.IsNullOrWhiteSpace(email))
            {
                MessageBox.Show("All fields are required.", "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            //creating a new var request with the inputs
            var signupRequest = new
            {
                username = username,
                password = password,
                email = email 
            };

            try
            {
                // building the login request with code 2
                byte[] buffer = CommunicationHelper.BuildMessage(2, signupRequest);
                _comm.Send(buffer);  //sending request to the server
                byte[] response = _comm.Receive();  //getting the server response
                //parsing the response
                var (code, json) = CommunicationHelper.ParseMessage(response);
                if (code == 2) //checking if response code and request code match
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (res.status == "SUCCESS")  //checking if status of response is success
                    {
                        // opening new menu window with the communicator and closing curr room
                        MenuWindow nextWindow = new MenuWindow(_comm);
                        nextWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Signup failed. Please check details.", "Signup Failed", MessageBoxButton.OK, MessageBoxImage.Warning);
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
                MessageBox.Show($"Signup failed: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            //opening new login window and closing curr window
            LoginWindow loginWindow = new LoginWindow(_comm);
            loginWindow.Show();
            this.Close(); 
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.RightsManagement;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Newtonsoft.Json;

namespace clientApp
{
    /// <summary>
    /// Interaction logic for LoginWindow.xaml
    /// </summary>
    public partial class LoginWindow : Window
    {
        private ClientCommunicator _comm;

        public LoginWindow(ClientCommunicator comm)
        {
            InitializeComponent();
            _comm = comm;  //setting communicator
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            //getting the username and password from input
            string username = UsernameTextBox.Text;
            string password = PasswordBox.Password;

            // checking if inputs are empty
            if (string.IsNullOrEmpty(username) || string.IsNullOrEmpty(password))
            {
                MessageBox.Show("Please enter both username and password.", "Input Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            //creating new login request var with the input fields
            var loginRequest = new
            {
                username = username,
                password = password
            };

            try
            {
                //building the request msg with login code 1 and the login request var
                byte[] buffer = CommunicationHelper.BuildMessage(1, loginRequest);
                _comm.Send(buffer);  //sending the request to the server

                //getting response from the server
                byte[] response = _comm.Receive();
                var (code, json) = CommunicationHelper.ParseMessage(response);

                if (code == 1) // if response code match the request code
                {
                    //converting the json
                    dynamic res = JsonConvert.DeserializeObject(json);
                    if (res.status == "SUCCESS")  //if status is success
                    {
                        // opening new menu window and closing curr
                        MenuWindow nextWindow = new MenuWindow(_comm); 
                        nextWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Login failed. Please check details.", "Login Failed", MessageBoxButton.OK, MessageBoxImage.Warning);
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
                MessageBox.Show($"Login failed: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }


        private void SignupButton_Click(object sender, RoutedEventArgs e)
        {
            //loading signup window and closing curr window
            SignupWindow signupWindow = new SignupWindow(_comm);
            signupWindow.Show();
            this.Close(); 
        }
    }
}

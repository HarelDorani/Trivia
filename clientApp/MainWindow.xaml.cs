using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace clientApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private ClientCommunicator _comm; 
        public MainWindow()
        {
            InitializeComponent();
            //initializing new instance of communicator and connecting to the server with port 2000 and ip 127.0.0.1
            _comm = new ClientCommunicator("127.0.0.1", 2000);
        }

        private void LetsPlayButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //loading new login window and closing curr
                LoginWindow loginWindow = new LoginWindow(_comm);
                loginWindow.Show();

                this.Close(); 
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to connect to server: {ex.Message}", "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

        }
    }
}
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
    /// Interaction logic for highScoreWindow.xaml
    /// </summary>
    public partial class highScoreWindow : Window
    {
        private ClientCommunicator _comm;
        public highScoreWindow(List<string> highScores, ClientCommunicator comm)
        {
            InitializeComponent();
            HighScoresListBox.ItemsSource = highScores;  //showing the high scores on the window
            _comm = comm; //setting communicator
        }
        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            //creating new menu window and closing curr
            MenuWindow menuWindow = new MenuWindow(_comm); 
            menuWindow.Show();
            this.Close();
        }
    }
}

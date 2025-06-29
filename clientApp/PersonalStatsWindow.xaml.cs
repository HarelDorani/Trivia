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
    /// Interaction logic for PersonalStatsWindow.xaml
    /// </summary>
    public partial class PersonalStatsWindow : Window
    {
        private ClientCommunicator _comm;
        public PersonalStatsWindow(List<string> personalStats, ClientCommunicator comm)
        {
            InitializeComponent();
            _comm = comm;  //setting the communicator
            StatsListBox.ItemsSource = personalStats;  //showing the personal stats on the screen

        }
        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            //opening new menu window and closing curr window
            MenuWindow menuWindow = new MenuWindow(_comm);
            menuWindow.Show();
            this.Close();
        }
    }
}

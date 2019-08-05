using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for ProgressWindow.xaml
    /// </summary>
    public partial class ProgressWindow : Window
    {
        public ProgressWindow()
        {
            InitializeComponent();
        }
        //this is on the task thread
        long lastpoll=0;
        public void UpdateProgressAsync(double prog, string msg = null, int frequencyInMs = 50)
        {
            //Update the PB UI
            //50ms is a good poll frequency.  Lokos smooth and very responsive.
            //Do not set to zero because this hangs the UI and your progress will be... the UI update!
            //freuencyInMs - prevent EXCESSIVE updates by updating the ui every so many MS
            if (Environment.TickCount - lastpoll > frequencyInMs)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                //prog is between [0,1]
                _pbProgress.Value = (double)_pbProgress.Maximum * (double)prog;
                    if (msg != null)
                    {
                        _lblStatusMsg.Content = msg;
                    }

                });
                lastpoll = Environment.TickCount;
            }
        }
        Task running = null;
        CancellationTokenSource tokenSource = null;
        public void Execute(Action<CancellationToken> stuffToDo)
        {
            Action<Task> continuewith = in_task =>
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    Close();
                });
            };
            tokenSource = new CancellationTokenSource();
            var token = tokenSource.Token;
            running = Task.Factory.StartNew(() => { stuffToDo(tokenSource.Token); }, tokenSource.Token).ContinueWith(continuewith);
            ShowDialog();
        }

        private  void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            //cancel task
            if (running != null)
            {
                if (tokenSource != null)
                {
                    tokenSource.Cancel();
                }
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsErrorDialog
{
    public partial class MainForm : Form
    {
        private static string _strErrorHeader;
        private static string _strErrorMessage;
        private static string _strLogFileDir;
        private static int _initHeight;
        private static int _initWidth;
        /************************************************************************/
        /* Note: All strings \r\n must be escaped                                                                     */
        /************************************************************************/
        public string fixString(string input)
        {
            //we someimtes have only \n.  Windows textbox needs \r\n
            input = input.Replace("\\r", "");
            input = input.Replace("\\n", "\r\n");

            return input;
        }
        public MainForm(string strErrorHeader, string strErrorMessage, string strLogFileDir)
        {
            InitializeComponent();

            strErrorHeader = fixString(strErrorHeader);
            strErrorMessage = fixString(strErrorMessage);
            strLogFileDir = fixString(strLogFileDir);

            _strErrorHeader = strErrorHeader;
            _strErrorMessage = strErrorMessage;
            _strLogFileDir = strLogFileDir;

            Text = _strErrorHeader;

            _initHeight = (_txtLogFilePath.Location.Y + _txtLogFilePath.Height) - _txtErrorDetails.Location.Y;
            _initWidth = 670;

            HideDetails();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            _txtErrorDetails.Text = _strErrorMessage;
            _txtLogFilePath.Text = _strLogFileDir;
        }
        private void HideDetails()
        {
            Height -= _initHeight;
            Width -= _initWidth;
            _txtErrorDetails.Hide();
            _txtLogFilePath.Hide();
            _lblErrorDetails.Text = "Show Details";

        }
        private void ShowDetails()
        {
            Height += _initHeight;
            Width += _initWidth;
            _txtErrorDetails.Show();
            _txtLogFilePath.Show();
            _lblErrorDetails.Text = "Hide Details";
        }

        private void _btnOk_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void _lblErrorDetails_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if (_txtErrorDetails.Visible == true)
                HideDetails();
            else
                ShowDetails();
        }
    }
}

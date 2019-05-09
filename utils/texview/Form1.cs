using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Proteus;

namespace TexView
{
    public partial class MainForm : Form
    {
        NetworkClient _objClient;
        const string _cPacketTag = "VT";

        public MainForm()
        {
            InitializeComponent();
            
        }
        #region Private:Methods
        private void SendRawMessage(string str, NetworkPacketType type)
        {
            //Make sure to pack the data before sending
            try
            {
                string pack =
                    NetworkUtils.PackPacketType(type, NetworkMode.Binary) //header
                    + str
                    ;
                _objClient.Send(pack, 10000, _cPacketTag);
            }
            catch (Exception ex)
            {
                Msg(ex.ToString());
            }
        }
        private void SendString(string str)
        {
            SendRawMessage(NetworkUtils.PackString(str, NetworkMode.Binary), NetworkPacketType.StringMessage);
        }
        private void SendLong64(long str)
        {
            SendRawMessage(NetworkUtils.PackLong(str, NetworkMode.Binary), NetworkPacketType.LongMessage);
        }
        private void ToggleConsole()
        {
            if (_txtOutput.Visible == false)
            {
                _txtOutput.Show();
                this.Height += _txtOutput.Height;
            }
            else
            {
                _txtOutput.Hide();
                this.Height -= _txtOutput.Height;
            }
        }
        private void Msg(string status)
        {
            this.BeginInvoke((Action)(() =>
            {
                Globals.Logger.LogInfo(status);
                _txtOutput.Text = Globals.Logger.GetLogString();
            }));
        }
        #endregion
        #region Private: UI
        private void MainForm_Load(object sender, EventArgs e)
        {
            Globals.InitializeGlobals("TexLog.log");
           // ToggleConsole();
        }
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (_objClient != null)
                _objClient.Disconnect();

            Application.Exit();
        }
        private void _btnTestSend_Click(object sender, EventArgs e)
        {
            SendString(_txtTestText.Text);
        }
        private void _btnConnect_Click(object sender, EventArgs e)
        {
            Msg("connecting");
            if (_objClient != null)
                _objClient.Disconnect();

            _objClient = new NetworkClient(_txtHostName.Text, NetworkMode.Binary);
            _objClient.Connect(System.Convert.ToInt32(_txtPort.Text), true);
            if(_objClient.IsConnected())
            {
                _btnConnect.Text = "Disconnect";
                Msg("connected");
            }
            else
                Msg("Failed to connect");
        }
        private void button1_Click(object sender, EventArgs e)
        {
            string msg = "";
            for(int i=0; i<10000; ++i)
            {
                SendString("Test");// _txtTestText.Text+i.ToString());
                msg += "Sent " + i + "\r\n";
            }
            Msg(msg);
        }
        private void _mnuViewConsole_Click(object sender, EventArgs e)
        {
            ToggleConsole(); 
        }

        #endregion
        Random _objRand = new Random();

        string RandStr(int iMaxLen) {
            string strRet = "";
            int rand = _objRand.Next()%iMaxLen;
            for (int i = 0; i < rand; i++)
                strRet += (char)(_objRand.Next() % 255);
            return strRet;
        }
        private void button2_Click(object sender, EventArgs e)
        {
            string msg = "";
            int iBytes = 0;
            for (int i = 0; i < 1000; ++i)
            {
                string st = RandStr(512);
                iBytes += st.Length;
                SendString(st);// _txtTestText.Text+i.ToString());
            }
            Msg("Sent " + iBytes.ToString() + " bytes") ;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string msg = "";
            string st = new string('0', (int)_nudRandomByteCount.Value);
            SendString(st);// _txtTestText.Text+i.ToString());

            Msg("Sent " + st.Length.ToString() + " bytes");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string msg = "";
            for (int i = 0; i < 1000; ++i)
            {
                SendString("Test " + i.ToString());// _txtTestText.Text+i.ToString());
            }
            Msg("Sent 1000");
        }

        private void _tmrUpdate_Tick(object sender, EventArgs e)
        {
            if (_objClient == null)
                return;

            //TODO: send a TexData message to the server every frame tick.

            Packet pack = _objClient.RecvBinary();
            if(pack!=null)
            {
                if(pack.Head == NetworkPacketType.TextureData)
                {
                    int n = 0;
                    n++;
                }
                else
                {
                    Msg("Got " + pack.Data);
                }
            }
        }
        private void button5_Click(object sender, EventArgs e)
        {
            SendRawMessage("NONE", NetworkPacketType.TextureData);
        }
    }
}

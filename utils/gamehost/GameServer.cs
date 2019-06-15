using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Proteus;
namespace GameHost
{
    public enum ServerState { Disconnected, Connecting, Connected, PostHandshake/*ProcCount*/, Dropped }

    //Server side construct only.
    public class GameServer
    {
        #region Public: Members

        public string ComputerName { get; set; }
        public ServerState ServerState { get; set; }  // If there was a network error &c that we cannt connect to him
        public int ErrorCount = 0; // number of errors this agent has hit.
        public bool IsLocalAgent { get { return ComputerName == System.Environment.MachineName; } private set { } }
        public bool AlreadyTriedToConnect = false;
        public int _intRequestIdentifierGenerator = 1000;//start at 1k so we are not confuesd
        public Object _objConnectLockObject = new Object();
        private bool _bInitialized = false;
        private PacketMakerTcp _objPacketMaker = new PacketMakerTcp();
        private System.Net.Sockets.Socket MySocket
        {
            get { return _objPacketMaker.Socket; }
            set { _objPacketMaker.Socket = value; }
        }
        private bool _blnDoHandshake = true;
        private List<string> _objCommandHistory = new List<string>();
        private Action _afterConnect = null;

        #endregion

        public GameServer()
        {
            ServerState = ServerState.Disconnected;
            ComputerName = System.Environment.MachineName;
        }

        #region NETWORKING


        public bool IsConnected()
        {
            return (MySocket != null) && (CheckConnected());
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="waitForResult">Block until we are connected to the server</param>
        /// <param name="doHandshake"></param>
        /// <returns>null if waitForResult is false, otherwise true/false depending on whether the connection succeeded</returns>
        public bool? Connect(bool doHandshake = true, Action afterConnect=null)
        {
            lock (_objConnectLockObject)
            {
                if (ServerState == ServerState.Connecting)
                {
                    return null;
                }
                if (IsConnected())
                {
                    return null;
                }
                ServerState = ServerState.Connecting;
                _blnDoHandshake = doHandshake;
            }

            _afterConnect = afterConnect;

            System.Net.IPAddress ip = NetworkUtils.GetIpAddress(ComputerName);

            MySocket = new System.Net.Sockets.Socket(
                System.Net.Sockets.AddressFamily.InterNetwork,
                System.Net.Sockets.SocketType.Stream,
                System.Net.Sockets.ProtocolType.IP);

            IAsyncResult result = MySocket.BeginConnect(ip, GameHostGlobals.GameServerPort, ConnectedToServer, this);


            //if (waitForResult)
            //{
            //    bool success = result.AsyncWaitHandle.WaitOne(GameHostGlobals.GameServerConnectionAttemptTimeoutMS, true);
            //    return success;
            //}
            return null;
        }

        public void Disconnect(bool blnWaitForResult = false)
        {
            lock (_objConnectLockObject)
            {
                ServerState = ServerState.Disconnected;
            }

            if (MySocket == null)
            {
                return;
            }
            if (IsConnected() == false)
            {
                return;
            }

            Globals.Logger.LogInfo("Disconnnecting..");
            try
            {
                IAsyncResult result = MySocket.BeginDisconnect(false, AgentDisconnected, this);
                if (blnWaitForResult)
                    result.AsyncWaitHandle.WaitOne(GameHostGlobals.GameHostDisconnectAttemptTimeoutMs, true);
            }
            catch (System.Net.Sockets.SocketException se)
            {
                Globals.Logger.LogInfo("Trying to disconnect we got " + se.ToString());
            }
        }
        public void Send(string buf, int iTimeout = GameHostGlobals.SendAndRecvTimeoutMs)
        {
            _objPacketMaker.SendPacket(buf, iTimeout);
            _objCommandHistory.Add(buf.Substring(0, 8));
        }
        public string Recv(int iTimeout = GameHostGlobals.SendAndRecvTimeoutMs)
        {
            string ret = _objPacketMaker.GetNextPacket(iTimeout);
            return ret;
        }

        private bool CheckConnected()
        {
            if (MySocket.Connected == false)
            {
                return false;
            }

            bool part1 = MySocket.Poll(1000, System.Net.Sockets.SelectMode.SelectRead);
            bool part2 = (MySocket.Available == 0);
            if (part1 && part2)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        private bool IsInit()
        {
            if (!_bInitialized)
            {
                return false;
            }
            return true;
        }
        private void DoHandshake()
        {
            if (ServerState != ServerState.PostHandshake)
            {
                string hostName;

                hostName = System.Environment.MachineName;
                string data = "HS000000" + Proteus.NetworkUtils.PackInt(4224) + NetworkUtils.PackString(hostName);

                Send(data);
                ServerState = ServerState.PostHandshake;
            }
        }
        private void ConnectedToServer(IAsyncResult ar)
        {
            //This is the asynchronous callback of the connection.
            GameServer ag = (GameServer)ar.AsyncState;

            try
            {
                ag.MySocket.EndConnect(ar);

                Proteus.Globals.Logger.LogInfo("Connected to " + ag.ComputerName);

                _afterConnect?.Invoke();

                bool dohs = false;
                lock (ag._objConnectLockObject)
                {
                    ag.ServerState = ServerState.Connected;
                    dohs = _blnDoHandshake;
                }

                if (dohs)
                {
                    ag.DoHandshake();
                }
            }
            catch (System.Net.Sockets.SocketException se)
            {
                if (se.SocketErrorCode == System.Net.Sockets.SocketError.ConnectionRefused)
                {
                    //Swallow - The agent is not running.
                    Proteus.Globals.Logger.LogInfo("  Agent " + ComputerName + " is not running.");
                }
                if (se.SocketErrorCode == System.Net.Sockets.SocketError.TimedOut)
                {
                    Proteus.Globals.Logger.LogInfo("  Agent " + ComputerName + " timed out.");
                }
                lock (ag._objConnectLockObject)
                {
                    ag.ServerState = ServerState.Disconnected;
                }
            }

        }
        private void AgentDisconnected(IAsyncResult ar)
        {
            Proteus.Globals.Logger.LogInfo("Disconnnected..");
        }
        #endregion

    }
}

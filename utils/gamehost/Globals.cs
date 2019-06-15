using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    public static class GameHostGlobals
    {
        public const int GameServerPort = 44244;

#if DEBUG
        public const int GameServerConnectionAttemptTimeoutMS = 60000;
        public const int GameHostDisconnectAttemptTimeoutMs = 60000;
        public const int SendAndRecvTimeoutMs = 60000;
#else
        public const int GameServerConnectionAttemptTimeoutMS = 30000;
        public const int GameHostDisconnectAttemptTimeoutMs = 2000;
        public const int SendAndRecvTimeoutMs = 2000;
#endif
    }
}

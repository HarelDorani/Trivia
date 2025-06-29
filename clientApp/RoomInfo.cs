using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace clientApp
{
    // this class represents an info of room
    public class RoomInfo
    {
        public int id { get; set; }
        public int maxPlayers { get; set; }
        public string name { get; set; }
        public int numOfQuestionsInGame { get; set; }
        public bool state { get; set; }
        public float timePerQuestion { get; set; }
    }

}

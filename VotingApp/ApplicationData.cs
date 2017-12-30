using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp
{
    public class ApplicationData
    {
        private ApplicationData() { }

        public static ApplicationData Current { get; } = new ApplicationData();

        public mainEntities DbContext { get => new mainEntities(); }

    }
}

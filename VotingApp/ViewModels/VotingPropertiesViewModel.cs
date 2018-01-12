using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class VotingPropertiesViewModel: Screen
    {
        public VotingPropertiesViewModel(Voting voting)
        {
            Voting = voting;
        }

        public Voting Voting { get; }

        public void Ok() => TryClose(true);

        public void Cancel() => TryClose();

    }
}

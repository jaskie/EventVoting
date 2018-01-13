using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.ViewModels
{
    public class VotingViewModel: PropertyChangedBase
    {
        private readonly Voting _voting;

        public VotingViewModel(Voting voting)
        {
            _voting = voting;
        }

        public string Name => _voting.Name;

        public DateTime? Start
        {
            get => _voting.Start;
            set
            {
                if (_voting.Start == value)
                    return;
                _voting.Start = value;
                NotifyOfPropertyChange();
            }
        }

        public DateTime? End
        {
            get => _voting.End;
            set
            {
                if (_voting.End == value)
                    return;
                _voting.End = value;
                NotifyOfPropertyChange();
            }
        }
    }
}

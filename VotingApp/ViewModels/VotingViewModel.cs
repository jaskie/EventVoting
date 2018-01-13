using Caliburn.Micro;
using System;

namespace EventVoting.VotingApp.ViewModels
{
    public class VotingViewModel: PropertyChangedBase
    {
        
        public VotingViewModel(Voting voting)
        {
            Voting = voting;
        }

        public Voting Voting { get; }

        public string Name => Voting.Name;

        public DateTime? Start
        {
            get => Voting.Start;
            set
            {
                if (Voting.Start == value)
                    return;
                Voting.Start = value;
                NotifyOfPropertyChange();
            }
        }

        public DateTime? End
        {
            get => Voting.End;
            set
            {
                if (Voting.End == value)
                    return;
                Voting.End = value;
                NotifyOfPropertyChange();
            }
        }
    }
}

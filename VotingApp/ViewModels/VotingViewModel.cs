using Caliburn.Micro;
using EventVoting.VotingApp.Hardware;
using System;
using System.Data.Entity;
using System.Linq;

namespace EventVoting.VotingApp.ViewModels
{
    public class VotingViewModel: PropertyChangedBase
    {

        private readonly LoRaTransceiver _loRaTransceiver;
        private readonly VotingDbContext _db;

        public VotingViewModel(Voting voting, VotingDbContext db, LoRaTransceiver loRaTransceiver)
        {
            Voting = voting;
            _loRaTransceiver = loRaTransceiver;
            _db = db;
            Votes = new BindableCollection<Vote>(_db.Vote.Where(v => v.IdVoting == voting.Id));
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
                NotifyOfPropertyChange(nameof(CanStartVoting));
                NotifyOfPropertyChange(nameof(CanFinishVoting));
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
                NotifyOfPropertyChange(nameof(CanFinishVoting));
            }
        }

        public void StartVoting()
        {
            _loRaTransceiver.StartVoting("Głosuj !");
            Start = DateTime.Now;
            _db.SaveChanges();
        }

        public bool CanStartVoting => Start == null;

        public void FinishVoting()
        {
            _loRaTransceiver.EndVoting("Zakończone");
            End = DateTime.Now;
            _db.SaveChanges();
        }

        public bool CanFinishVoting => Start != null && End == null;

        public BindableCollection<Vote> Votes { get; } 
        
        internal void VoteResponse(Device device, int result)
        {
            if (Votes.Any(v => v.IdAppliance == device.Id))
                return;
            var vote = new Vote
            {
                IdVoting = Voting.Id,
                IdAppliance = device.Id,
                Result = result
            };
            Votes.Add(vote);
            _db.Vote.Add(vote);
            _db.SaveChanges();
        }

    }
}

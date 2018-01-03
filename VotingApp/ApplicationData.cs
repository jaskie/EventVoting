using EventVoting.VotingApp.Database;

namespace EventVoting.VotingApp
{
    public class ApplicationData
    {
        private ApplicationData()
        {
            System.Data.Entity.Database.SetInitializer(new DatabaseInitializer());
            DbContext = new VotingDbContext();
        }

        public static ApplicationData Current { get; } = new ApplicationData();

        public VotingDbContext DbContext { get; } 
    }
}

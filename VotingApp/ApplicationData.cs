using EventVoting.VotingApp.Database;

namespace EventVoting.VotingApp
{
    public class ApplicationData
    {

        private ApplicationData()
        {
            System.Data.Entity.Database.SetInitializer(new DatabaseInitializer());
        }

        public static ApplicationData Current { get; } = new ApplicationData();

    }
}

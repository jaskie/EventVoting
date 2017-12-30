using EventVoting.VotingApp.DomainModel;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Data.Entity.ModelConfiguration.Conventions;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.Database
{
    public class VotingContext: DbContext
    {
        public VotingContext()
        {
            System.Data.Entity.Database.SetInitializer<VotingContext>(null);
        }

        public DbSet<Appliance> Appliances { get; set; }

        public DbSet<Vote> Votes { get; set; }

        public DbSet<Voting> Votings { get; set; }

    }
}

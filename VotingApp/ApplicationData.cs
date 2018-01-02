using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp
{
    public class ApplicationData
    {
        private ApplicationData()
        {
            //DbContext = new DatabaseContext();
            DbConfiguration.LoadConfiguration(typeof(DatabaseContext));
            Database.SetInitializer(new CreateDatabaseIfNotExists<DatabaseContext>());
            DbContext = new DatabaseContext();
            {
                using (var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream("EventVoting.VotingApp.Database.Schema.sql"))
                using (var reader = new StreamReader(stream))
                {
                    var sqlText = reader.ReadToEnd();
                    DbContext.Database.ExecuteSqlCommand(sqlText);
                }
            }

        }

        public static ApplicationData Current { get; } = new ApplicationData();

        public DatabaseContext DbContext { get; } 
    }
}

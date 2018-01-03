using System.Data.Common;
using System.Data.Entity;
using System.IO;
using System.Reflection;

namespace EventVoting.VotingApp.Database
{
    public class DatabaseInitializer: IDatabaseInitializer<VotingDbContext>
    {
        public void InitializeDatabase(VotingDbContext context)
        {
            using (var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream("EventVoting.VotingApp.Database.Schema.sql"))
            using (var reader = new StreamReader(stream))
            {
                ExecuteScript(context.Database.Connection, reader.ReadToEnd());
            }
        }

        private void ExecuteScript(DbConnection connection, string script)
        {
            if (connection.State != System.Data.ConnectionState.Open)
                connection.Open();
            var command = connection.CreateCommand();
            command.CommandText = script;
            command.ExecuteNonQuery();
        }
    }
}

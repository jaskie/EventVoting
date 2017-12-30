using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.DomainModel
{
    public class Vote
    {
        [Key]
        public Int64 Id { get; set; }

        [Required]
        public Int64 IdVoting { get; set; }

        [Required]
        public Int64 IdAppliance { get; set; }

        public Int32? Result { get; set; }

    }
}

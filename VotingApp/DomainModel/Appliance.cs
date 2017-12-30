using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EventVoting.VotingApp.DomainModel
{
    public class Appliance
    {
        [Key]
        public Int64 Id { get; set; }

        [Required]
        public Int32 Type { get; set; }

        [Required]
        public Byte[] DeviceId { get; set; }

    }
}

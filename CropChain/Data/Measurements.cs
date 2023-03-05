using System.ComponentModel.DataAnnotations;

namespace CropChain.Data
{
    public class Measurements
    {

        public int Id { get; set; }
        public decimal Data { get; set; }
        public DateTime Timestamp { get; set; }
        public String Data_Type { get; set; }
        public int Sensor_Id { get; set; }

    }
}

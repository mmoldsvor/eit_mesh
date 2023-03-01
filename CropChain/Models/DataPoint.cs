namespace CropChain.Models
{
    public class DataPoint
    {
        public float Data { get; }
        public DateTime Timestamp { get; }

        public DataPoint(float data, DateTime timestamp)
        {
            this.Data = data;
            this.Timestamp = timestamp;
        }

        override public string ToString() => Timestamp.ToString() + "  " + Data.ToString();

    }
}

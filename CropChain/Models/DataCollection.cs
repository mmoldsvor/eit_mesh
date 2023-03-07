﻿namespace CropChain.Models
{
    public class DataCollection : IObservable<DataPoint>
    {

        private List<DataPoint> measurements;
        private float alertLimit;
        private List<IObserver<DataPoint>> observers;
        
        private string data_Type;
        private int sensor_Id;


        public DataCollection(List<DataPoint> measurements, string data_Type, int sensor_Id)
        {
            this.measurements = measurements;
            this.alertLimit = 0.3f;
            this.observers = new List<IObserver<DataPoint>>();
            this.data_Type = data_Type;
            this.sensor_Id = sensor_Id;
        }

        public IDisposable Subscribe(IObserver<DataPoint> observer)
        {
            if (!observers.Contains(observer))
            {
                observers.Add(observer);
                foreach (DataPoint dataPoint in measurements)
                {
                    observer.OnNext(dataPoint);
                }
            }
            return new Unsubscriber<DataPoint>(observers, observer);
        }

        internal class Unsubscriber<DataPoint> : IDisposable
        {

            private List<IObserver<DataPoint>> observers;
            private IObserver<DataPoint> observer;

            internal Unsubscriber(List<IObserver<DataPoint>> observers, IObserver<DataPoint> observer)
            {
                this.observers = observers;
                this.observer = observer;
            }

            public void Dispose()
            {
                if (observers.Contains(observer))
                {
                    observers.Remove(observer);
                }
            }
        }


        public DataPoint getLatest()
        {
            return measurements.Last();
        }

        public List<DataPoint> getAll()
        {
            return measurements;
        }

        public float getLimit()
        {
            return alertLimit;
        }

        public string getData_Type()
        { return data_Type; }

        public int getSensor_Id()
        { return sensor_Id; }

    }
}

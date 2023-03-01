namespace CropChain.Models
{
    public class DataCollection : IObservable<DataPoint>
    {

        private List<DataPoint> measurements;
        private float alertLimit;
        private List<IObserver<DataPoint>> observers;


        public DataCollection(List<DataPoint> measurements)
        {
            this.measurements = measurements;
            this.alertLimit = 0.3f;
            this.observers = new List<IObserver<DataPoint>>();
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

    }
}

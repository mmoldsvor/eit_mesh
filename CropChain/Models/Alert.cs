namespace CropChain.Models
{
    public class Alarm : IObserver<DataPoint>
    {

        private IDisposable unsubscriber;

        public Alarm()
        {

        }

        public virtual void Subscribe(DataCollection dataCollection)
        {
            this.unsubscriber = dataCollection.Subscribe(this);
        }

        public virtual void Unsubscribe()
        {
            unsubscriber.Dispose();
        }

        public void OnCompleted()
        {
            throw new NotImplementedException();
        }

        public void OnError(Exception error)
        {
            throw new NotImplementedException();
        }

        public void OnNext(DataPoint dataPoint)
        {
            throw new NotImplementedException();
        }
    }
}

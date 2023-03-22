from sqlalchemy import create_engine, Column, Integer, DateTime, DECIMAL, NCHAR
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
from datetime import datetime, timezone
import random
from serial_parser import serial_generator

SENSOR_HUMIDITY = 'humid'
SENSOR_TEMPERATURE = 'temp'

SENSOR_TYPES = {
    SENSOR_HUMIDITY,
    SENSOR_TEMPERATURE
}

conn_string = 'mssql+pyodbc://@(localdb)\\mssqllocaldb/CropChainContext-aa803c14-79a1-48b2-ae43-6f0a9e7c0e00?driver=ODBC+Driver+17+for+SQL+Server&trusted_connection=yes'
engine = create_engine(conn_string, pool_reset_on_return=None)

Session = sessionmaker(bind=engine)
session = Session()

connection = engine.connect()
Base = declarative_base()


class Measurements(Base):
    __tablename__ = 'measurements'

    id = Column(Integer, primary_key=True)
    data = Column(DECIMAL)
    timestamp = Column(DateTime)
    data_type = Column(NCHAR)
    sensor_id = Column(Integer)


def insert_measurement(data, data_type, id, timestamp=None):
    if (data_type in SENSOR_TYPES):
        current_timestamp = timestamp
        if timestamp is None:
            current_timestamp = datetime.now(timezone.utc)
        session.add(Measurements(
            data = data,
            timestamp = current_timestamp,
            data_type = data_type,
            sensor_id = id
        ))
        session.commit()
    else:
        print('Sensor type was not valid')

if __name__ == '__main__':
    # random_data = random.uniform(0, 1)
    yield_object = serial_generator()
    data = yield_object.next()
    insert_measurement(data[0], SENSOR_TEMPERATURE, data[2])
    insert_measurement(data[1], SENSOR_HUMIDITY, data[2])

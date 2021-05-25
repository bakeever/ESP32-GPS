"""
EECE 396 Topics
Bryce Keever
December 9th, 2020
Description: This code imports latitude and longitude
data from an excel spreadsheet and plots the path
"""
import pandas as pd
#matplotlib inline
import matplotlib.pyplot as plt

Location = pd.read_excel(
    r'C:\Users\bkeev\OneDrive\Documents\Fall 2020\Topics\Final Project\Coordinates_Test_1_Sec.xlsx')
Lat = Location.Latitude
Long = Location.Longitude

plt.scatter(Long, Lat)
plt.title('GPS Coordinates')
plt.xlabel('Longitude')
plt.ylabel('Latitude')
plt.show()
import pandas
from sklearn import model_selection
from sklearn.linear_model import LogisticRegression
import sys

def findBestconfig( filename ):
   dataframe = pandas.read_csv( filename )
   array = dataframe.values
   x = array[:,-1]

   print("Performance summary based on", len(array), "evaluations:")
   print("Min: ", str( x.min() ), "s")
   print("Max: ", str( x.max() ), "s")
   print("Mean: ", str( x.mean() ), "s")

   headers = dataframe.columns.values
   l = ", ".join( headers[:-3] )
   l = l + " and " + headers[-3]
   print("The best configurations (for the smallest time) of ", l, " is:\n")

   mn = x.min()
   for i in range(len(array)):
      if x[i] == mn:
         for h, v in zip(  headers[:-3], array[i,:] ):
            print( "%s \t : %s" % (h, v ) )
      

def main():
   if len( sys.argv ) < 2 :
      print( "Please provide input file (csv)" )
      exit( -1 )
   findBestconfig( sys.argv[1] )

if __name__ == "__main__":
   main()
   

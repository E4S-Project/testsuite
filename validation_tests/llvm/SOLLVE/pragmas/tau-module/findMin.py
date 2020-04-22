import pandas

dataframe = pandas.read_csv("results.csv")
array = dataframe.values
nbelem = len( dataframe.columns ) - 2

minrow = dataframe[ 'elapsed_sec' ].idxmin()

print("Performance summary based on", len(array), "evaluations:")

print("Min: ", dataframe[ 'elapsed_sec' ].min(), "s" )
print("Max: ", dataframe[ 'elapsed_sec' ].max(), "s" )
print("Mean: ", dataframe[ 'elapsed_sec' ].mean(), "s" )

print("The best configurations (for the smallest time) of %s and %s is:\n" % ( ', '.join( dataframe.columns.values[:-3] ), dataframe.columns.values[-3] ) )

# print( dataframe.columns.values[:-3] )
for field in sorted( dataframe.columns.values[:-2] ):
   print( field, ": ", dataframe.loc[ minrow ][ field ]  )


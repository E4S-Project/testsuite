import cinemasci

# create the database object
cdb = cinemasci.new("cdb", {"path": "my_database.cdb"}) 

# initialize
cdb.initialize()

# add entries in any order, and with fully or partially populated attributes 
id = cdb.add_entry({'FILE02': '0002.png', 'time': '1.0', 'phi': '10.0', 'theta': '0.0'})
id = cdb.add_entry({'time': '0.0', 'phi': '0.0', 'theta': '0.0', 'FILE': '0000.png'})
id = cdb.add_entry({'time': '1.0', 'phi': '10.0', 'theta': '0.0', 'FILE01': '0001.png'})
id = cdb.add_entry({'time': '1.0', 'FILE': '0003.png'})

# finalize (writes out metadata for entries)
cdb.finalize()

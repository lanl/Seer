# This script expands attributes (field data) to cell data
# Copy this script to the python programmable filter
# Filters -> Alphabetical -> Python Programmable Filter

# Replace the value of attribute with the attribute you want
attribute = "PAPI_TOT_CYC"
pdi = self.GetInput()

numCells = pdi.GetNumberOfCells()
numRanks = inputs[0].FieldData["numRanks"]

tempData = inputs[0].CellData["cell-data-scalar"]

for i in range(0, numCells):
    rankInfo = inputs[0].CellData['rank'][i]

    for r in range(0, numRanks):
        attribute_rank = attribute + "_" + str(r)

        if (rankInfo == r):
            tempData[i] = inputs[0].FieldData[attribute_rank]
            break

output.CellData.append(tempData, attribute)




"""
attribute = "PAPI_BR_PRC"
pdi = self.GetInput()

numPoints = pdi.GetNumberOfPoints()
numRanks = inputs[0].FieldData["numRanks"]

tempData = inputs[0].PointData["id"]

for i in range(0, numPoints):
    rankInfo = inputs[0].PointData['rank'][i]

    for r in range(0, numRanks):
        attribute_rank = attribute + "_" + str(r)

        if (rankInfo == r):
            tempData[i] = inputs[0].FieldData[attribute_rank]
            break

output.PointData.append(tempData, attribute)
"""


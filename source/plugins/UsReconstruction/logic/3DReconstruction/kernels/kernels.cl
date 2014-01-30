kernel void test(void)
{
	size_t i = get_global_id(0);
	i++;
}


//float getNormalVectorDistance(imagePosition, voxelPosition)
//{
//	return ;
//}
//
//getClosesPointOnPlane()
//{
//
//}
//
//kernel void findClosesPlanes(global inputImagePositions, int4 voxelPosition, int numberOfClosesPlanes)
//{
//	for(int i=0; i < numberOfInputImages; i++)
//	{
//		distance = normalVector = getNormalVectorDistance(image[i], voxelPosition);
//		map = save(i, distance);
//	    if(isBetterThanAlreadyFound(distance))
//			closesPointOnPlane = getClosesPointOnPlane(normalVector, voxelPosition);
//	    	if(isInsideImage(closesPointOnPlane, inputImagePositions[i], sizeOfInputImages))
//	    		keep();
//	}
//	return best(map, numberOfClosesPlanes)
//}

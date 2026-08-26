#include "BinarySpacePartition.h"
#include "RoomVisualizer.h"

bool RoomVisualizer::visualize(
    const BinarySpacePartition& bsp,
    const std::string& outputPath) const
{
    return visualize(bsp.getSpace(), bsp.getRoomCount(), bsp.getStartRoomId(), bsp.getEndRoomId(), outputPath);
}

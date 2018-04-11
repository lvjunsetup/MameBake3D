#include "stdafx.h"
#include "PhysicsDirect.h"

#include "PhysicsClientSharedMemory.h"
#include "../CommonInterfaces/CommonGUIHelperInterface.h"
#include "SharedMemoryCommands.h"
#include "PhysicsCommandProcessorInterface.h"


#include "LinearMath/btHashMap.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "../../Extras/Serialize/BulletFileLoader/btBulletFile.h"
#include "../../Extras/Serialize/BulletFileLoader/autogenerated/bullet.h"
#include "BodyJointInfoUtility.h"
#include <string>

struct BodyJointInfoCache2
{
	std::string m_baseName;
	btAlignedObjectArray<b3JointInfo> m_jointInfo;
};



struct PhysicsDirectInternalData
{
	DummyGUIHelper m_noGfx;

	btAlignedObjectArray<char> m_serverDNA;
	SharedMemoryCommand m_command;
	SharedMemoryStatus m_serverStatus;
	bool m_hasStatus;
	bool m_verboseOutput;
	
	btAlignedObjectArray<TmpFloat3> m_debugLinesFrom;
    btAlignedObjectArray<TmpFloat3> m_debugLinesTo;
    btAlignedObjectArray<TmpFloat3> m_debugLinesColor;

	btHashMap<btHashInt,BodyJointInfoCache2*> m_bodyJointMap;
    btHashMap<btHashInt,b3UserConstraint> m_userConstraintInfoMap;
	
	char    m_bulletStreamDataServerToClient[SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE];

	int m_cachedCameraPixelsWidth;
	int m_cachedCameraPixelsHeight;
	btAlignedObjectArray<unsigned char> m_cachedCameraPixelsRGBA;
	btAlignedObjectArray<float> m_cachedCameraDepthBuffer;
	btAlignedObjectArray<int> m_cachedSegmentationMask;
	
    btAlignedObjectArray<b3ContactPointData> m_cachedContactPoints;
	btAlignedObjectArray<b3OverlappingObject> m_cachedOverlappingObjects;

	btAlignedObjectArray<b3VisualShapeData> m_cachedVisualShapes;
    btAlignedObjectArray<b3VRControllerEvent> m_cachedVREvents;

	btAlignedObjectArray<b3RayHitInfo>	m_raycastHits;

	PhysicsCommandProcessorInterface* m_commandProcessor;
	bool m_ownsCommandProcessor;

	PhysicsDirectInternalData()
		:m_hasStatus(false),
		m_verboseOutput(false),
		m_ownsCommandProcessor(false)
	{
	}
};

PhysicsDirect::PhysicsDirect(PhysicsCommandProcessorInterface* physSdk, bool passSdkOwnership)
{
	m_data = new PhysicsDirectInternalData;
	m_data->m_commandProcessor = physSdk;
	m_data->m_ownsCommandProcessor = passSdkOwnership;
}

PhysicsDirect::~PhysicsDirect()
{
	if (m_data->m_commandProcessor->isConnected())
	{
		m_data->m_commandProcessor->disconnect();
	}	
	if (m_data->m_ownsCommandProcessor)
	{
		delete m_data->m_commandProcessor;
	}

	resetData();

	

	delete m_data;
}

void PhysicsDirect::resetData()
{
	m_data->m_debugLinesFrom.clear();
	m_data->m_debugLinesTo.clear();
	m_data->m_debugLinesColor.clear();
	for (int i = 0; i<m_data->m_bodyJointMap.size(); i++)
	{
		BodyJointInfoCache2** bodyJointsPtr = m_data->m_bodyJointMap.getAtIndex(i);
		if (bodyJointsPtr && *bodyJointsPtr)
		{
			BodyJointInfoCache2* bodyJoints = *bodyJointsPtr;
			for (int j = 0; j<bodyJoints->m_jointInfo.size(); j++) {
				if (bodyJoints->m_jointInfo[j].m_jointName)
				{
					free(bodyJoints->m_jointInfo[j].m_jointName);
				}
				if (bodyJoints->m_jointInfo[j].m_linkName)
				{
					free(bodyJoints->m_jointInfo[j].m_linkName);
				}
			}
			delete (*bodyJointsPtr);
		}
	}
	m_data->m_bodyJointMap.clear();
	m_data->m_userConstraintInfoMap.clear();
}

// return true if connection succesfull, can also check 'isConnected'
bool PhysicsDirect::connect()
{
	bool connected = m_data->m_commandProcessor->connect();
	m_data->m_commandProcessor->setGuiHelper(&m_data->m_noGfx);


	if (connected)
	//also request serialization data
	{
		SharedMemoryCommand command;
		command.m_type = CMD_REQUEST_INTERNAL_DATA;
		bool hasStatus = m_data->m_commandProcessor->processCommand(command, m_data->m_serverStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
		if (hasStatus)
		{
			postProcessStatus(m_data->m_serverStatus);
		}
		else
		{
			int timeout = 1024 * 1024 * 1024;
			while ((!hasStatus) && (timeout-- > 0))
			{
				const  SharedMemoryStatus* stat = processServerStatus();
				if (stat)
				{
					hasStatus = true;
				}
			}
		}
	}

	return connected;
}

// return true if connection succesfull, can also check 'isConnected'
bool PhysicsDirect::connect(struct GUIHelperInterface* guiHelper)
{
	bool connected = m_data->m_commandProcessor->connect();

	m_data->m_commandProcessor->setGuiHelper(guiHelper);
	
	return connected;
}

void PhysicsDirect::renderScene()
{
	m_data->m_commandProcessor->renderScene();
}

void PhysicsDirect::debugDraw(int debugDrawMode)
{
	m_data->m_commandProcessor->physicsDebugDraw(debugDrawMode);
}

////todo: rename to 'disconnect'
void PhysicsDirect::disconnectSharedMemory()
{
	m_data->m_commandProcessor->disconnect();
	m_data->m_commandProcessor->setGuiHelper(0);
}

bool PhysicsDirect::isConnected() const
{
	return m_data->m_commandProcessor->isConnected();
}

// return non-null if there is a status, nullptr otherwise
const  SharedMemoryStatus* PhysicsDirect::processServerStatus()
{
	
	if (!m_data->m_hasStatus)
	{
		m_data->m_hasStatus = m_data->m_commandProcessor->receiveStatus(m_data->m_serverStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
	}

	SharedMemoryStatus* stat = 0;
	if (m_data->m_hasStatus)
	{
		stat = &m_data->m_serverStatus;
		
		postProcessStatus(m_data->m_serverStatus);
		
		m_data->m_hasStatus = false;
	}
	return stat;
}

SharedMemoryCommand* PhysicsDirect::getAvailableSharedMemoryCommand()
{
	return &m_data->m_command;
}

bool PhysicsDirect::canSubmitCommand() const
{
	return m_data->m_commandProcessor->isConnected();
}

bool PhysicsDirect::processDebugLines(const struct SharedMemoryCommand& orgCommand)
{
	SharedMemoryCommand command = orgCommand;

	const SharedMemoryStatus& serverCmd = m_data->m_serverStatus;

	do
	{

		bool hasStatus = m_data->m_commandProcessor->processCommand(command,m_data->m_serverStatus,&m_data->m_bulletStreamDataServerToClient[0],SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);

		int timeout = 1024 * 1024 * 1024;
		while ((!hasStatus) && (timeout-- > 0))
		{
			const  SharedMemoryStatus* stat = processServerStatus();
			if (stat)
			{
				hasStatus = true;
			}
		}

		m_data->m_hasStatus = hasStatus;

		if (hasStatus)
		{
			btAssert(m_data->m_serverStatus.m_type == CMD_DEBUG_LINES_COMPLETED);

			if (m_data->m_verboseOutput) 
			{
				b3Printf("Success receiving %d debug lines",
							serverCmd.m_sendDebugLinesArgs.m_numDebugLines);
			}

			int numLines = serverCmd.m_sendDebugLinesArgs.m_numDebugLines;
			float* linesFrom =
				(float*)&m_data->m_bulletStreamDataServerToClient[0];
			float* linesTo =
				(float*)(&m_data->m_bulletStreamDataServerToClient[0] + 
						numLines * 3 * sizeof(float));
			float* linesColor =
				(float*)(&m_data->m_bulletStreamDataServerToClient[0] +
							2 * numLines * 3 * sizeof(float));

			m_data->m_debugLinesFrom.resize(serverCmd.m_sendDebugLinesArgs.m_startingLineIndex +
											numLines);
			m_data->m_debugLinesTo.resize(serverCmd.m_sendDebugLinesArgs.m_startingLineIndex +
											numLines);
			m_data->m_debugLinesColor.resize(
				serverCmd.m_sendDebugLinesArgs.m_startingLineIndex + numLines);

			for (int i = 0; i < numLines; i++) 
			{
				TmpFloat3 from = CreateTmpFloat3(linesFrom[i * 3], linesFrom[i * 3 + 1],
													linesFrom[i * 3 + 2]);
				TmpFloat3 to =
					CreateTmpFloat3(linesTo[i * 3], linesTo[i * 3 + 1], linesTo[i * 3 + 2]);
				TmpFloat3 color = CreateTmpFloat3(linesColor[i * 3], linesColor[i * 3 + 1],
													linesColor[i * 3 + 2]);

				m_data
					->m_debugLinesFrom[serverCmd.m_sendDebugLinesArgs.m_startingLineIndex + i] =
					from;
				m_data->m_debugLinesTo[serverCmd.m_sendDebugLinesArgs.m_startingLineIndex + i] =
					to;
				m_data->m_debugLinesColor[serverCmd.m_sendDebugLinesArgs.m_startingLineIndex +
											i] = color;
			}

			if (serverCmd.m_sendDebugLinesArgs.m_numRemainingDebugLines > 0)
			{
				m_data->m_hasStatus = false;

				command.m_type = CMD_REQUEST_DEBUG_LINES;
				command.m_requestDebugLinesArguments.m_startingLineIndex =
					serverCmd.m_sendDebugLinesArgs.m_numDebugLines +
					serverCmd.m_sendDebugLinesArgs.m_startingLineIndex;
			}
		}

	} while (serverCmd.m_sendDebugLinesArgs.m_numRemainingDebugLines > 0);
	
	return m_data->m_hasStatus;
}

bool PhysicsDirect::processVisualShapeData(const struct SharedMemoryCommand& orgCommand)
{
	SharedMemoryCommand command = orgCommand;
	const SharedMemoryStatus& serverCmd = m_data->m_serverStatus;

	do
	{
		bool hasStatus = m_data->m_commandProcessor->processCommand(command, m_data->m_serverStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);

		int timeout = 1024 * 1024 * 1024;
		while ((!hasStatus) && (timeout-- > 0))
		{
			const  SharedMemoryStatus* stat = processServerStatus();
			if (stat)
			{
				hasStatus = true;
			}
		}

		m_data->m_hasStatus = hasStatus;
		if (hasStatus)
		{
			
			if (m_data->m_verboseOutput)
			{
				b3Printf("Visual Shape Information Request OK\n");
			}
			int startVisualShapeIndex = serverCmd.m_sendVisualShapeArgs.m_startingVisualShapeIndex;
			int numVisualShapesCopied = serverCmd.m_sendVisualShapeArgs.m_numVisualShapesCopied;
			m_data->m_cachedVisualShapes.resize(startVisualShapeIndex + numVisualShapesCopied);
			b3VisualShapeData* shapeData = (b3VisualShapeData*)&m_data->m_bulletStreamDataServerToClient[0];
			for (int i = 0; i < numVisualShapesCopied; i++)
			{
				m_data->m_cachedVisualShapes[startVisualShapeIndex + i] = shapeData[i];
			}
						
			if (serverCmd.m_sendVisualShapeArgs.m_numRemainingVisualShapes >0 && serverCmd.m_sendVisualShapeArgs.m_numVisualShapesCopied)
			{
				m_data->m_hasStatus = false;

				command.m_type = CMD_REQUEST_VISUAL_SHAPE_INFO;
				command.m_requestVisualShapeDataArguments.m_startingVisualShapeIndex = serverCmd.m_sendVisualShapeArgs.m_startingVisualShapeIndex + serverCmd.m_sendVisualShapeArgs.m_numVisualShapesCopied;
				command.m_requestVisualShapeDataArguments.m_bodyUniqueId = serverCmd.m_sendVisualShapeArgs.m_bodyUniqueId;
			}
		}
	} while (serverCmd.m_sendVisualShapeArgs.m_numRemainingVisualShapes > 0 && serverCmd.m_sendVisualShapeArgs.m_numVisualShapesCopied);
	
	return m_data->m_hasStatus;
}

bool PhysicsDirect::processOverlappingObjects(const struct SharedMemoryCommand& orgCommand)
{
	SharedMemoryCommand command = orgCommand;

	const SharedMemoryStatus& serverCmd = m_data->m_serverStatus;

	do
	{
		bool hasStatus = m_data->m_commandProcessor->processCommand(command, m_data->m_serverStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);

		int timeout = 1024 * 1024 * 1024;
		while ((!hasStatus) && (timeout-- > 0))
		{
			const  SharedMemoryStatus* stat = processServerStatus();
			if (stat)
			{
				hasStatus = true;
			}
		}


		m_data->m_hasStatus = hasStatus;
		if (hasStatus)
		{
			if (m_data->m_verboseOutput)
			{
				b3Printf("Overlapping Objects Request OK\n");
			}

			int startOverlapIndex = serverCmd.m_sendOverlappingObjectsArgs.m_startingOverlappingObjectIndex;
			int numOverlapCopied = serverCmd.m_sendOverlappingObjectsArgs.m_numOverlappingObjectsCopied;
			m_data->m_cachedOverlappingObjects.resize(startOverlapIndex + numOverlapCopied);
			b3OverlappingObject* objects = (b3OverlappingObject*)&m_data->m_bulletStreamDataServerToClient[0];

			for (int i = 0; i < numOverlapCopied; i++)
			{
				m_data->m_cachedOverlappingObjects[startOverlapIndex + i] = objects[i];
			}

			if (serverCmd.m_sendOverlappingObjectsArgs.m_numRemainingOverlappingObjects > 0 && serverCmd.m_sendOverlappingObjectsArgs.m_numOverlappingObjectsCopied)
			{
				m_data->m_hasStatus = false;
				command.m_type = CMD_REQUEST_AABB_OVERLAP;
				command.m_requestOverlappingObjectsArgs.m_startingOverlappingObjectIndex = serverCmd.m_sendOverlappingObjectsArgs.m_startingOverlappingObjectIndex + serverCmd.m_sendOverlappingObjectsArgs.m_numOverlappingObjectsCopied;
			}

		}
	} while (serverCmd.m_sendOverlappingObjectsArgs.m_numRemainingOverlappingObjects > 0 && serverCmd.m_sendOverlappingObjectsArgs.m_numOverlappingObjectsCopied);

	return m_data->m_hasStatus;

}



bool PhysicsDirect::processContactPointData(const struct SharedMemoryCommand& orgCommand)
{
    SharedMemoryCommand command = orgCommand;
    
    const SharedMemoryStatus& serverCmd = m_data->m_serverStatus;
    
    do
    {
        bool hasStatus = m_data->m_commandProcessor->processCommand(command,m_data->m_serverStatus,&m_data->m_bulletStreamDataServerToClient[0],SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
        
		int timeout = 1024 * 1024 * 1024;
		while ((!hasStatus) && (timeout-- > 0))
		{
			const  SharedMemoryStatus* stat = processServerStatus();
			if (stat)
			{
				hasStatus = true;
			}
		}


		m_data->m_hasStatus = hasStatus;
        if (hasStatus)
        {
            if (m_data->m_verboseOutput)
            {
                b3Printf("Contact Point Information Request OK\n");
            }
            int startContactIndex = serverCmd.m_sendContactPointArgs.m_startingContactPointIndex;
            int numContactsCopied = serverCmd.m_sendContactPointArgs.m_numContactPointsCopied;
            
            m_data->m_cachedContactPoints.resize(startContactIndex+numContactsCopied);
            
            b3ContactPointData* contactData = (b3ContactPointData*)&m_data->m_bulletStreamDataServerToClient[0];
            
            for (int i=0;i<numContactsCopied;i++)
            {
                m_data->m_cachedContactPoints[startContactIndex+i] = contactData[i];
            }
            
            if (serverCmd.m_sendContactPointArgs.m_numRemainingContactPoints>0 && serverCmd.m_sendContactPointArgs.m_numContactPointsCopied)
            {
    
				m_data->m_hasStatus = false;

				command.m_type = CMD_REQUEST_CONTACT_POINT_INFORMATION;
                command.m_requestContactPointArguments.m_startingContactPointIndex = serverCmd.m_sendContactPointArgs.m_startingContactPointIndex+serverCmd.m_sendContactPointArgs.m_numContactPointsCopied;
                command.m_requestContactPointArguments.m_objectAIndexFilter = -1;
                command.m_requestContactPointArguments.m_objectBIndexFilter = -1;
                
            }

        }
    } while (serverCmd.m_sendContactPointArgs.m_numRemainingContactPoints > 0 && serverCmd.m_sendContactPointArgs.m_numContactPointsCopied);
    
    return m_data->m_hasStatus;

}


bool PhysicsDirect::processCamera(const struct SharedMemoryCommand& orgCommand)
{
	SharedMemoryCommand command = orgCommand;

	const SharedMemoryStatus& serverCmd = m_data->m_serverStatus;

	do
	{

		bool hasStatus = m_data->m_commandProcessor->processCommand(command,m_data->m_serverStatus,&m_data->m_bulletStreamDataServerToClient[0],SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
		
		int timeout = 1024 * 1024 * 1024;
		while ((!hasStatus) && (timeout-- > 0))
		{
			const  SharedMemoryStatus* stat = processServerStatus();
			if (stat)
			{
				hasStatus = true;
			}
		}


		m_data->m_hasStatus = hasStatus;
		if (hasStatus)
		{
			btAssert(m_data->m_serverStatus.m_type == CMD_CAMERA_IMAGE_COMPLETED);

			if (m_data->m_verboseOutput) 
			{
				b3Printf("Camera image OK\n");
			}

			int numBytesPerPixel = 4;//RGBA
			int numTotalPixels = serverCmd.m_sendPixelDataArguments.m_startingPixelIndex+
				serverCmd.m_sendPixelDataArguments.m_numPixelsCopied+
				serverCmd.m_sendPixelDataArguments.m_numRemainingPixels;

			m_data->m_cachedCameraPixelsWidth = 0;
			m_data->m_cachedCameraPixelsHeight = 0;

            int numPixels = serverCmd.m_sendPixelDataArguments.m_imageWidth*serverCmd.m_sendPixelDataArguments.m_imageHeight;

            m_data->m_cachedCameraPixelsRGBA.reserve(numPixels*numBytesPerPixel);
			m_data->m_cachedCameraDepthBuffer.resize(numTotalPixels);
			m_data->m_cachedSegmentationMask.resize(numTotalPixels);
			m_data->m_cachedCameraPixelsRGBA.resize(numTotalPixels*numBytesPerPixel);
                
                
			unsigned char* rgbaPixelsReceived =
                (unsigned char*)&m_data->m_bulletStreamDataServerToClient[0];
			
			float* depthBuffer = (float*)&(m_data->m_bulletStreamDataServerToClient[serverCmd.m_sendPixelDataArguments.m_numPixelsCopied*4]);
			int* segmentationMaskBuffer = (int*)&(m_data->m_bulletStreamDataServerToClient[serverCmd.m_sendPixelDataArguments.m_numPixelsCopied*8]);
			
          //  printf("pixel = %d\n", rgbaPixelsReceived[0]);
                
			for (int i=0;i<serverCmd.m_sendPixelDataArguments.m_numPixelsCopied;i++)
			{
				m_data->m_cachedCameraDepthBuffer[i + serverCmd.m_sendPixelDataArguments.m_startingPixelIndex] = depthBuffer[i];
			}
			for (int i=0;i<serverCmd.m_sendPixelDataArguments.m_numPixelsCopied;i++)
			{
				m_data->m_cachedSegmentationMask[i + serverCmd.m_sendPixelDataArguments.m_startingPixelIndex] = segmentationMaskBuffer[i];
			}
			for (int i=0;i<serverCmd.m_sendPixelDataArguments.m_numPixelsCopied*numBytesPerPixel;i++)
			{
				m_data->m_cachedCameraPixelsRGBA[i + serverCmd.m_sendPixelDataArguments.m_startingPixelIndex*numBytesPerPixel] 
					= rgbaPixelsReceived[i];
			}

			if (serverCmd.m_sendPixelDataArguments.m_numRemainingPixels > 0 && serverCmd.m_sendPixelDataArguments.m_numPixelsCopied)
			{
				
				m_data->m_hasStatus = false;

				// continue requesting remaining pixels
				command.m_type = CMD_REQUEST_CAMERA_IMAGE_DATA;
				command.m_requestPixelDataArguments.m_startPixelIndex = 
					serverCmd.m_sendPixelDataArguments.m_startingPixelIndex + 
					serverCmd.m_sendPixelDataArguments.m_numPixelsCopied;
				
			} else
			{
				m_data->m_cachedCameraPixelsWidth = serverCmd.m_sendPixelDataArguments.m_imageWidth;
				m_data->m_cachedCameraPixelsHeight = serverCmd.m_sendPixelDataArguments.m_imageHeight;
			}	
		}
	}  while (serverCmd.m_sendPixelDataArguments.m_numRemainingPixels > 0 && serverCmd.m_sendPixelDataArguments.m_numPixelsCopied);
	
	return m_data->m_hasStatus;


}


void PhysicsDirect::processBodyJointInfo(int bodyUniqueId, const SharedMemoryStatus& serverCmd)
{

	BodyJointInfoCache2** cachePtr = m_data->m_bodyJointMap[bodyUniqueId];
	//don't process same bodyUniqueId multiple times
	if (cachePtr)
	{
		return;
	}

    bParse::btBulletFile bf(
        &m_data->m_bulletStreamDataServerToClient[0],
        serverCmd.m_numDataStreamBytes);
	if (m_data->m_serverDNA.size())
	{
		bf.setFileDNA(false, &m_data->m_serverDNA[0], m_data->m_serverDNA.size());
	}
	else
	{
		bf.setFileDNAisMemoryDNA();
	}
    bf.parse(false);
	
    BodyJointInfoCache2* bodyJoints = new BodyJointInfoCache2;
    m_data->m_bodyJointMap.insert(bodyUniqueId,bodyJoints);

    for (int i = 0; i < bf.m_multiBodies.size(); i++) 
    {
        int flag = bf.getFlags();
        if ((flag & bParse::FD_DOUBLE_PRECISION) != 0) 
        {
            Bullet::btMultiBodyDoubleData* mb =
                (Bullet::btMultiBodyDoubleData*)bf.m_multiBodies[i];
            
            if (mb->m_baseName)
            {
                bodyJoints->m_baseName = mb->m_baseName;
            }
            addJointInfoFromMultiBodyData(mb,bodyJoints, m_data->m_verboseOutput);
        } else 
        {
            Bullet::btMultiBodyFloatData* mb =
                (Bullet::btMultiBodyFloatData*)bf.m_multiBodies[i];
			
            
            if (mb->m_baseName)
            {
                bodyJoints->m_baseName = mb->m_baseName;
            }
            addJointInfoFromMultiBodyData(mb,bodyJoints, m_data->m_verboseOutput);
        }
    }
    if (bf.ok()) 
	{
        if (m_data->m_verboseOutput) 
        {
            b3Printf("Received robot description ok!\n");
        }
    } else 
    {
        b3Warning("Robot description not received");
    }
}

void PhysicsDirect::postProcessStatus(const struct SharedMemoryStatus& serverCmd)
{
	switch (serverCmd.m_type)
	{
	
	case CMD_REQUEST_RAY_CAST_INTERSECTIONS_COMPLETED:
	{
		if (m_data->m_verboseOutput)
		{
			b3Printf("Raycast completed");
		}
		m_data->m_raycastHits.clear();
		for (int i=0;i<serverCmd.m_raycastHits.m_numRaycastHits;i++)
		{
			m_data->m_raycastHits.push_back(serverCmd.m_raycastHits.m_rayHits[i]);
		}
		break;
	}
	case CMD_REQUEST_VR_EVENTS_DATA_COMPLETED:
	{

		if (m_data->m_verboseOutput)
		{
			b3Printf("Request VR Events completed");
		}
		m_data->m_cachedVREvents.clear();
		for (int i=0;i< serverCmd.m_sendVREvents.m_numVRControllerEvents;i++)
		{
			m_data->m_cachedVREvents.push_back(serverCmd.m_sendVREvents.m_controllerEvents[i]);
		}
		break;
	}

	case CMD_REQUEST_INTERNAL_DATA_COMPLETED:
	{
		if (serverCmd.m_numDataStreamBytes)
		{
			int numStreamBytes = serverCmd.m_numDataStreamBytes;
			m_data->m_serverDNA.resize(numStreamBytes);
			for (int i = 0; i < numStreamBytes; i++)
			{
				m_data->m_serverDNA[i] = m_data->m_bulletStreamDataServerToClient[i];
			}
		}
		break;
	}
	case CMD_RESET_SIMULATION_COMPLETED:
	{
		resetData();
		break;
	}

	case CMD_USER_CONSTRAINT_INFO_COMPLETED:
    case CMD_USER_CONSTRAINT_COMPLETED:
    {
        int cid = serverCmd.m_userConstraintResultArgs.m_userConstraintUniqueId;
        m_data->m_userConstraintInfoMap.insert(cid,serverCmd.m_userConstraintResultArgs);
        break;
    }
    case CMD_REMOVE_USER_CONSTRAINT_COMPLETED:
    {
        int cid = serverCmd.m_userConstraintResultArgs.m_userConstraintUniqueId;
        m_data->m_userConstraintInfoMap.remove(cid);
        break;
    }
	case CMD_CHANGE_USER_CONSTRAINT_COMPLETED:
	{
        int cid = serverCmd.m_userConstraintResultArgs.m_userConstraintUniqueId;
		b3UserConstraint* userConstraintPtr = m_data->m_userConstraintInfoMap[cid];
		if (userConstraintPtr)
		{
			const b3UserConstraint* serverConstraint = &serverCmd.m_userConstraintResultArgs;
			if (serverCmd.m_updateFlags & USER_CONSTRAINT_CHANGE_PIVOT_IN_B)
			{
					userConstraintPtr->m_childFrame[0] = serverConstraint->m_childFrame[0];
					userConstraintPtr->m_childFrame[1] = serverConstraint->m_childFrame[1];
					userConstraintPtr->m_childFrame[2] = serverConstraint->m_childFrame[2];
			}
			if (serverCmd.m_updateFlags & USER_CONSTRAINT_CHANGE_FRAME_ORN_IN_B)
			{
				userConstraintPtr->m_childFrame[3] = serverConstraint->m_childFrame[3];
				userConstraintPtr->m_childFrame[4] = serverConstraint->m_childFrame[4];
				userConstraintPtr->m_childFrame[5] = serverConstraint->m_childFrame[5];
				userConstraintPtr->m_childFrame[6] = serverConstraint->m_childFrame[6];
			}
			if (serverCmd.m_updateFlags & USER_CONSTRAINT_CHANGE_MAX_FORCE)
			{
				userConstraintPtr->m_maxAppliedForce = serverConstraint->m_maxAppliedForce;
			}
		}
		break;
	}

	case CMD_SYNC_BODY_INFO_COMPLETED:
	case CMD_MJCF_LOADING_COMPLETED:
	case CMD_SDF_LOADING_COMPLETED:
	{
		//we'll stream further info from the physics server
		//so serverCmd will be invalid, make a copy

		int numConstraints = serverCmd.m_sdfLoadedArgs.m_numUserConstraints;
		for (int i=0;i<numConstraints;i++)
		{
			int constraintUid = serverCmd.m_sdfLoadedArgs.m_userConstraintUniqueIds[i];
			SharedMemoryCommand infoRequestCommand;
			infoRequestCommand.m_type = CMD_USER_CONSTRAINT;
			infoRequestCommand.m_updateFlags = USER_CONSTRAINT_REQUEST_INFO;
            infoRequestCommand.m_userConstraintArguments.m_userConstraintUniqueId = constraintUid;
			SharedMemoryStatus infoStatus;
			bool hasStatus = m_data->m_commandProcessor->processCommand(infoRequestCommand, infoStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
					

			int timeout = 1024 * 1024 * 1024;
			while ((!hasStatus) && (timeout-- > 0))
			{
				hasStatus = m_data->m_commandProcessor->receiveStatus(infoStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
			}

			if (hasStatus)
			{
				int cid = infoStatus.m_userConstraintResultArgs.m_userConstraintUniqueId;
				m_data->m_userConstraintInfoMap.insert(cid,infoStatus.m_userConstraintResultArgs);
			}
		}

		int numBodies = serverCmd.m_sdfLoadedArgs.m_numBodies;
		for (int i = 0; i<numBodies; i++)
		{
			int bodyUniqueId = serverCmd.m_sdfLoadedArgs.m_bodyUniqueIds[i];
			SharedMemoryCommand infoRequestCommand;
			infoRequestCommand.m_type = CMD_REQUEST_BODY_INFO;
			infoRequestCommand.m_sdfRequestInfoArgs.m_bodyUniqueId = bodyUniqueId;
			SharedMemoryStatus infoStatus;
			bool hasStatus = m_data->m_commandProcessor->processCommand(infoRequestCommand, infoStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
					

			int timeout = 1024 * 1024 * 1024;
			while ((!hasStatus) && (timeout-- > 0))
			{
				hasStatus = m_data->m_commandProcessor->receiveStatus(infoStatus, &m_data->m_bulletStreamDataServerToClient[0], SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
			}

			if (hasStatus)
			{
				processBodyJointInfo(bodyUniqueId, infoStatus);
			}
		}
		break;
	}
	case CMD_URDF_LOADING_COMPLETED:
	{

		if (serverCmd.m_numDataStreamBytes > 0)
		{
			int bodyIndex = serverCmd.m_dataStreamArguments.m_bodyUniqueId;
			processBodyJointInfo(bodyIndex, serverCmd);
		}
		break;
	}
	case CMD_BULLET_LOADING_FAILED:
	{
		b3Warning("Couldn't load .bullet file");
		break;
	}
	case CMD_BULLET_LOADING_COMPLETED:
	{
		break;
	}
	
	case CMD_REMOVE_USER_CONSTRAINT_FAILED:
	{
		b3Warning("removeConstraint failed");
		break;
	}
	case CMD_CHANGE_USER_CONSTRAINT_FAILED:
	{
		b3Warning("changeConstraint failed");
		break;
	}

	case CMD_USER_CONSTRAINT_FAILED:
	{
		b3Warning("createConstraint failed");
		break;
	}
	default:
	{
		//b3Warning("Unknown server status type");
	}
	};


}
bool PhysicsDirect::submitClientCommand(const struct SharedMemoryCommand& command)
{
	if (command.m_type==CMD_REQUEST_DEBUG_LINES)
	{			
		return processDebugLines(command);
	}

	if (command.m_type==CMD_REQUEST_CAMERA_IMAGE_DATA)
	{
		return processCamera(command);
	}
    if (command.m_type == CMD_REQUEST_CONTACT_POINT_INFORMATION)
    {
        return processContactPointData(command);
    }

	if (command.m_type == CMD_REQUEST_VISUAL_SHAPE_INFO)
	{
		return processVisualShapeData(command);
	}
	if (command.m_type == CMD_REQUEST_AABB_OVERLAP)
	{
		return processOverlappingObjects(command);
	}

	bool hasStatus = m_data->m_commandProcessor->processCommand(command,m_data->m_serverStatus,&m_data->m_bulletStreamDataServerToClient[0],SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);
	m_data->m_hasStatus = hasStatus;
	/*if (hasStatus)
	{
		postProcessStatus(m_data->m_serverStatus);
		m_data->m_hasStatus = false;
	}
	*/
	return hasStatus;
}

int PhysicsDirect::getNumBodies() const
{
	return m_data->m_bodyJointMap.size();
}

int PhysicsDirect::getNumUserConstraints() const
{
    return m_data->m_userConstraintInfoMap.size();
}

int PhysicsDirect::getUserConstraintInfo(int constraintUniqueId, struct b3UserConstraint&info) const
{
    b3UserConstraint* constraintPtr =m_data->m_userConstraintInfoMap[constraintUniqueId];
    if (constraintPtr)
    {
        info = *constraintPtr;
        return 1;
    }
    return 0;
}



int PhysicsDirect::getBodyUniqueId(int serialIndex) const
{
	if ((serialIndex >= 0) && (serialIndex < getNumBodies()))
	{
		return m_data->m_bodyJointMap.getKeyAtIndex(serialIndex).getUid1();
	}
	return -1;
}

bool PhysicsDirect::getBodyInfo(int bodyUniqueId, struct b3BodyInfo& info) const
{
	BodyJointInfoCache2** bodyJointsPtr = m_data->m_bodyJointMap[bodyUniqueId];
	if (bodyJointsPtr && *bodyJointsPtr)
	{
		BodyJointInfoCache2* bodyJoints = *bodyJointsPtr;
		info.m_baseName = bodyJoints->m_baseName.c_str();
		return true;
	}
	
	return false;
}

int PhysicsDirect::getNumJoints(int bodyIndex) const
{
	BodyJointInfoCache2** bodyJointsPtr = m_data->m_bodyJointMap[bodyIndex];
	if (bodyJointsPtr && *bodyJointsPtr)
	{
		BodyJointInfoCache2* bodyJoints = *bodyJointsPtr;
		return bodyJoints->m_jointInfo.size(); 
	}
	btAssert(0);
	return 0;
}

bool PhysicsDirect::getJointInfo(int bodyIndex, int jointIndex, struct b3JointInfo& info) const
{
	BodyJointInfoCache2** bodyJointsPtr = m_data->m_bodyJointMap[bodyIndex];
	if (bodyJointsPtr && *bodyJointsPtr)
	{
		BodyJointInfoCache2* bodyJoints = *bodyJointsPtr;
        if ((jointIndex >=0) && (jointIndex < bodyJoints->m_jointInfo.size()))
        {
            info = bodyJoints->m_jointInfo[jointIndex];
            return true;
        }
	}
    return false;
}

///todo: move this out of the
void PhysicsDirect::setSharedMemoryKey(int key)
{
	//m_data->m_physicsServer->setSharedMemoryKey(key);
	//m_data->m_physicsClient->setSharedMemoryKey(key);
}

void PhysicsDirect::uploadBulletFileToSharedMemory(const char* data, int len)
{
	//m_data->m_physicsClient->uploadBulletFileToSharedMemory(data,len);
}

int PhysicsDirect::getNumDebugLines() const
{
	return m_data->m_debugLinesFrom.size();
}

const float* PhysicsDirect::getDebugLinesFrom() const
{
	if (getNumDebugLines())
	{
		return &m_data->m_debugLinesFrom[0].m_x;
	}
	return 0;
}
const float* PhysicsDirect::getDebugLinesTo() const
{
	if (getNumDebugLines())
	{
		return &m_data->m_debugLinesTo[0].m_x;
	}
	return 0;
}
const float* PhysicsDirect::getDebugLinesColor() const
{
	if (getNumDebugLines())
	{
		return &m_data->m_debugLinesColor[0].m_x;
	}
	return 0;
}

void PhysicsDirect::getCachedCameraImage(b3CameraImageData* cameraData)
{
	if (cameraData)
	{
		cameraData->m_pixelWidth = m_data->m_cachedCameraPixelsWidth;
		cameraData->m_pixelHeight = m_data->m_cachedCameraPixelsHeight;
		cameraData->m_depthValues = m_data->m_cachedCameraDepthBuffer.size() ? &m_data->m_cachedCameraDepthBuffer[0] : 0;
		cameraData->m_rgbColorData = m_data->m_cachedCameraPixelsRGBA.size() ? &m_data->m_cachedCameraPixelsRGBA[0] : 0;
		cameraData->m_segmentationMaskValues = m_data->m_cachedSegmentationMask.size()? &m_data->m_cachedSegmentationMask[0] : 0;
	}
}

void PhysicsDirect::getCachedContactPointInformation(struct b3ContactInformation* contactPointData)
{
    contactPointData->m_numContactPoints = m_data->m_cachedContactPoints.size();
    contactPointData->m_contactPointData = contactPointData->m_numContactPoints? &m_data->m_cachedContactPoints[0] : 0;
    
}

void PhysicsDirect::getCachedOverlappingObjects(struct b3AABBOverlapData* overlappingObjects)
{
	overlappingObjects->m_numOverlappingObjects = m_data->m_cachedOverlappingObjects.size();
	overlappingObjects->m_overlappingObjects = m_data->m_cachedOverlappingObjects.size() ?
		&m_data->m_cachedOverlappingObjects[0] : 0;
}


void PhysicsDirect::getCachedVisualShapeInformation(struct b3VisualShapeInformation* visualShapesInfo)
{
	visualShapesInfo->m_numVisualShapes = m_data->m_cachedVisualShapes.size();
	visualShapesInfo->m_visualShapeData = visualShapesInfo->m_numVisualShapes ? &m_data->m_cachedVisualShapes[0] : 0;
}

void PhysicsDirect::getCachedVREvents(struct b3VREventsData* vrEventsData)
{
	vrEventsData->m_numControllerEvents = m_data->m_cachedVREvents.size();
	vrEventsData->m_controllerEvents = vrEventsData->m_numControllerEvents?
							&m_data->m_cachedVREvents[0] : 0;
}

void PhysicsDirect::getCachedRaycastHits(struct b3RaycastInformation* raycastHits)
{
	raycastHits->m_numRayHits = m_data->m_raycastHits.size();
	raycastHits->m_rayHits = raycastHits->m_numRayHits? &m_data->m_raycastHits[0] : 0;
}

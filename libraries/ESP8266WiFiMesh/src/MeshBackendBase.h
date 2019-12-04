/*
  MeshBackendBase
 
  Copyright (c) 2015 Julian Fell and 2019 Anders Löfgren. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __MESHBACKENDBASE_H__
#define __MESHBACKENDBASE_H__

#include <ESP8266WiFi.h>
#include "TransmissionResult.h"

const String ESP8266_MESH_EMPTY_STRING = "";

typedef enum
{
  MB_TCP_IP = 0,
  MB_ESP_NOW = 1
} mesh_backend_t;

class MeshBackendBase {

protected:

  typedef std::function<String(const String &, MeshBackendBase &)> requestHandlerType;
  typedef std::function<transmission_status_t(const String &, MeshBackendBase &)> responseHandlerType;
  typedef std::function<void(int, MeshBackendBase &)> networkFilterType;

public:

  MeshBackendBase(requestHandlerType requestHandler, responseHandlerType responseHandler, networkFilterType networkFilter, mesh_backend_t classType);

  virtual ~MeshBackendBase();

  /** 
  * A vector that contains the NetworkInfo for each WiFi network to connect to. 
  * The connectionQueue vector is cleared before each new scan and filled via the networkFilter callback function once the scan completes.
  * WiFi connections will start with connectionQueue[0] and then incrementally proceed to higher vector positions. 
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  static std::vector<NetworkInfo> connectionQueue;

  /** 
  * A vector with the TransmissionResult for each AP to which a transmission was attempted during the latest attemptTransmission call.
  * The latestTransmissionOutcomes vector is cleared before each new transmission attempt.
  * Connection attempts are indexed in the same order they were attempted.
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  static std::vector<TransmissionResult> latestTransmissionOutcomes;

  /**
   * @returns True if latest transmission was successful (i.e. latestTransmissionOutcomes is not empty and all entries have transmissionStatus TS_TRANSMISSION_COMPLETE). False otherwise.
   */
  static bool latestTransmissionSuccessful();

  /**
   * Initialises the node.
   */
  virtual void begin() = 0;

  /**
   * Each AP requires a separate server port. If two AP:s are using the same server port, they will not be able to have both server instances active at the same time.
   * This is managed automatically by the activateAP method.
   */
  void activateAP();
  void deactivateAP();
  void restartAP();

  /**
   * Get the MeshBackendBase instance currently in control of the ESP8266 AP.
   * Note that the result will be nullptr when there is no active AP controller.
   * If another instance takes control over the AP after the pointer is created,
   * the created pointer will still point to the old AP instance.
   * 
   * @returns A pointer to the MeshBackendBase instance currently in control of the ESP8266 AP,
   *          or nullptr if there is no active AP controller.
   */
  static MeshBackendBase *getAPController();
  
  /**
   * Check if this MeshBackendBase instance is in control of the ESP8266 AP.
   * 
   * @returns True if this MeshBackendBase instance is in control of the ESP8266 AP. False otherwise.
   */
  bool isAPController();

  /**
   * Change the WiFi channel used by this MeshBackendBase instance. 
   * Will also change the WiFi channel for the active AP if this MeshBackendBase instance is the current AP controller and it is possible to change channel.
   * 
   * WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
   * This can cause problems if several MeshBackendBase instances exist on the same ESP8266 and use different WiFi channels. 
   * In such a case, whenever the station of one MeshBackendBase instance connects to an AP, it will silently force the 
   * WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   * make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * 
   * @param newWiFiChannel The WiFi channel to change to. Valid values are integers from 1 to 13.
   *                          
   */
  void setWiFiChannel(uint8 newWiFiChannel);
  uint8 getWiFiChannel();

  /**
   * Change the SSID used by this MeshBackendBase instance. 
   * Will also change the SSID for the active AP if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDPrefix The first part of the new SSID.
   * @param newSSIDRoot The middle part of the new SSID.
   * @param newSSIDSuffix The last part of the new SSID.
   */  
  void setSSID(const String &newSSIDPrefix = ESP8266_MESH_EMPTY_STRING, const String &newSSIDRoot = ESP8266_MESH_EMPTY_STRING, 
               const String &newSSIDSuffix = ESP8266_MESH_EMPTY_STRING);
  String getSSID();

  /**
   * Change the first part of the SSID used by this MeshBackendBase instance. 
   * Will also change the first part of the SSID for the active AP if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDPrefix The new first part of the SSID.
   */  
  void setSSIDPrefix(const String &newSSIDPrefix);
  String getSSIDPrefix();

  /**
   * Change the middle part of the SSID used by this MeshBackendBase instance. 
   * Will also change the middle part of the SSID for the active AP if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDPrefix The new middle part of the SSID.
   */
  void setSSIDRoot(const String &newSSIDRoot);
  String getSSIDRoot();

  /**
   * Change the last part of the SSID used by this MeshBackendBase instance. 
   * Will also change the last part of the SSID for the active AP if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDSuffix The new last part of the SSID.
   */  
  void setSSIDSuffix(const String &newSSIDSuffix);
  String getSSIDSuffix();

  /**
   * Change the mesh name used by this MeshBackendBase instance.
   * Will also change the mesh name for the active AP if this MeshBackendBase instance is the current AP controller.
   * Used as alias for setSSIDPrefix by default. Feel free to override this method in a subclass if your mesh name is not equal to SSIDPrefix.
   *
   * @param newMeshName The mesh name to change to.                        
   */
  virtual void setMeshName(const String &newMeshName);
  virtual String getMeshName();
      
  /**
   * Change the node id used by this MeshBackendBase instance. 
   * Will also change the node id for the active AP if this MeshBackendBase instance is the current AP controller.
   * Used as alias for setSSIDSuffix by default. Feel free to override this method in a subclass if your node id is not equal to SSIDSuffix.
   *
   * @param newNodeID The node id to change to.                        
   */
  virtual void setNodeID(const String &newNodeID);
  virtual String getNodeID();

  /**
   * Set the password used when connecting to other AP:s and when other nodes connect to the AP of this node.
   * Will also change the setting for the active AP if this MeshBackendBase instance is the current AP controller.
   * 
   * @param newMeshPassword The password to use.
   */
  void setMeshPassword(const String &newMeshPassword);
  String getMeshPassword();
  
  /**
   * Set the message that will be sent to other nodes when calling attemptTransmission.
   * 
   * @param newMessage The message to send.
   */
  void setMessage(const String &newMessage);
  String getMessage();

  virtual void attemptTransmission(const String &message, bool scan = true, bool scanAllWiFiChannels = false) = 0;

  void setRequestHandler(requestHandlerType requestHandler);
  requestHandlerType getRequestHandler();
  
  void setResponseHandler(responseHandlerType responseHandler);
  responseHandlerType getResponseHandler();
    
  void setNetworkFilter(networkFilterType networkFilter);
  networkFilterType getNetworkFilter();

  /**
   * Set whether scan results from this MeshBackendBase instance will include WiFi networks with hidden SSIDs.
   * This is false by default.
   * The SSID field of a found hidden network will be blank in the scan results.
   * WiFi.isHidden(networkIndex) can be used to verify that a found network is hidden.
   *
   * @param scanHidden If true, WiFi networks with hidden SSIDs will be included in scan results.
   */
  void setScanHidden(bool scanHidden);
  bool getScanHidden();

  /**
   * Set whether the AP controlled by this MeshBackendBase instance will have a WiFi network with hidden SSID.
   * This is false by default.
   * Will also change the setting for the active AP if this MeshBackendBase instance is the current AP controller.
   *
   * @param apHidden If true, the WiFi network created will have a hidden SSID.
   */
  void setAPHidden(bool apHidden);
  bool getAPHidden();

  /**
   * Set whether the normal events occurring in the library will be printed to Serial or not. Off by default.
   * This setting is separate for each mesh instance.
   * 
   * @param enabled If true, library Serial prints are activated.
   */
  virtual void setVerboseModeState(bool enabled);
  virtual bool verboseMode();
  
  /**
   * Only print stringToPrint if verboseMode() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  virtual void verboseModePrint(const String &stringToPrint, bool newline = true);
  
  /**
   * Set whether the warnings occurring in the library will be printed to Serial or not. On by default.
   * This setting will affect all mesh instances.
   * 
   * @param printEnabled If true, warning Serial prints from the library are activated.
   */
  static void setPrintWarnings(bool printEnabled);
  static bool printWarnings();

  /**
   * Only print stringToPrint if printWarnings() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  static void warningPrint(const String &stringToPrint, bool newline = true);

  mesh_backend_t getClassType();

protected:

  virtual void scanForNetworks(bool scanAllWiFiChannels);
  virtual void printAPInfo(const int apNetworkIndex, const String &apSSID, const int apWiFiChannel);

  /**
   * Called just before we activate the AP.
   * Put _server.stop() in deactivateAPHook() in case you use _server.begin() here.
   */
  virtual void activateAPHook();

  /**
   * Called just before we deactivate the AP.
   * Put _server.stop() here in case you use _server.begin() in activateAPHook().
   */
  virtual void deactivateAPHook();

  void setClassType(mesh_backend_t classType);

private:

  mesh_backend_t _classType;

  static MeshBackendBase *apController;

  String _SSID;
  String _SSIDPrefix;
  String _SSIDRoot;
  String _SSIDSuffix;
  String _meshPassword;
  uint8 _meshWiFiChannel;
  bool _verboseMode;
  String _message = ESP8266_MESH_EMPTY_STRING;
  bool _scanHidden = false;
  bool _apHidden = false;

  requestHandlerType _requestHandler;
  responseHandlerType _responseHandler;
  networkFilterType _networkFilter;

  static bool _printWarnings;
};

#endif

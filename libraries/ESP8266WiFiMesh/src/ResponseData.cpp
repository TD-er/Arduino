/*
 * Copyright (C) 2019 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ResponseData.h"

ResponseData::ResponseData(const String &message, const uint8_t recipientMac[6], uint64_t requestID, uint32_t creationTimeMs) : 
  TimeTracker(creationTimeMs), _message(message), _requestID(requestID)
{      
  storeRecipientMac(recipientMac);
}

ResponseData::ResponseData(const ResponseData &other) 
  : TimeTracker(other), _message(other.getMessage()), _requestID(other.getRequestID())
{
  storeRecipientMac(other.getRecipientMac());
}

ResponseData & ResponseData::operator=(const ResponseData &other)
{
  if(this != &other)
  {
    TimeTracker::operator=(other);
    _message = other.getMessage();
    _requestID = other.getRequestID();
    storeRecipientMac(other.getRecipientMac());
  }
  
  return *this;
}

void ResponseData::storeRecipientMac(const uint8_t newRecipientMac[6])
{
  if(newRecipientMac != nullptr)
  {
    if(_recipientMac == nullptr)
    {
      _recipientMac = _recipientMacArray;
    }
    
    for(int i = 0; i < 6; i++)
    {
      _recipientMac[i] = newRecipientMac[i];
    }
  }
  else
  {
    _recipientMac = nullptr;
  }
}

void ResponseData::setRecipientMac(const uint8_t recipientMac[6]) { storeRecipientMac(recipientMac); }
const uint8_t *ResponseData::getRecipientMac() const { return _recipientMac; }

void ResponseData::setMessage(String &message) { _message = message; }
String ResponseData::getMessage() const { return _message; }

void ResponseData::setRequestID(uint64_t requestID) { _requestID = requestID; }
uint64_t ResponseData::getRequestID() const { return _requestID; }

// Copyright 2014 SAP AG.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http: //www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific
// language governing permissions and limitations under the License.

#include "error.h"
#include "rfcio.h"
using namespace v8;


Local<Object> RfcLibError(RFC_ERROR_INFO* errorInfo, Isolate* isolate) {
  Local<Value> e = Exception::Error(wrapString(errorInfo->message)->ToString());;
  Local<Object> errorObj = e->ToObject();

  errorObj->Set(String::NewFromUtf8(isolate, "code"), Integer::New(isolate, errorInfo->code));
  errorObj->Set(String::NewFromUtf8(isolate, "key"), wrapString(errorInfo->key));

  return errorObj;
}


Local<Object> AbapError(RFC_ERROR_INFO* errorInfo, Isolate* isolate) {
  Local<Value> e = Exception::Error(wrapString(errorInfo->message)->ToString());
  Local<Object> errorObj = e->ToObject();

  errorObj->Set(String::NewFromUtf8(isolate, "code"), Integer::New(isolate, errorInfo->code));
  errorObj->Set(String::NewFromUtf8(isolate, "key"), wrapString(errorInfo->key));
  errorObj->Set(String::NewFromUtf8(isolate, "message"), wrapString(errorInfo->message));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgClass"), wrapString(errorInfo->abapMsgClass));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgType"), wrapString(errorInfo->abapMsgType));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgNumber"), wrapString(errorInfo->abapMsgNumber));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgV1"), wrapString(errorInfo->abapMsgV1));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgV2"), wrapString(errorInfo->abapMsgV2));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgV3"), wrapString(errorInfo->abapMsgV3));
  errorObj->Set(String::NewFromUtf8(isolate, "abapMsgV4"), wrapString(errorInfo->abapMsgV4));

  return errorObj;
}

Local<Value> wrapError(RFC_ERROR_INFO* errorInfo) {
  Isolate* isolate = Isolate::GetCurrent();
  EscapableHandleScope scope(isolate);
  Local <Value> e;
  char cBuf[256];

  //errorInfo->group = OK;

  switch (errorInfo->group) {
    case OK:                              // 0: should never happen
      e = Exception::Error(String::NewFromUtf8(isolate, "node-rfc internal error: Error handling invoked with the RFC error group OK"));
      return scope.Escape(e->ToObject());
      break;

    case LOGON_FAILURE:                   // 3: Error message raised when logon fails
    case COMMUNICATION_FAILURE:           // 4: Problems with the network connection (or backend broke down and killed the connection)
    case EXTERNAL_RUNTIME_FAILURE:        // 5: Problems in the RFC runtime of the external program (i.e "this" library)
      return scope.Escape(RfcLibError(errorInfo, isolate));
      break;

	case ABAP_APPLICATION_FAILURE:        // 1: ABAP Exception raised in ABAP function modules
	case ABAP_RUNTIME_FAILURE:            // 2: ABAP Message raised in ABAP function modules or in ABAP runtime of the backend (e.g Kernel)
	case EXTERNAL_APPLICATION_FAILURE:    // 6: Problems in the external program (e.g in the external server implementation)
	case EXTERNAL_AUTHORIZATION_FAILURE:  // 7: Problems raised in the authorization check handler provided by the external server implementation
      return scope.Escape(AbapError(errorInfo, isolate));
      break;
  }

  // unknown error group
  sprintf(cBuf, "node-rfc internal error: Unknown error group: %u", errorInfo->group);
  e = Exception::Error(String::NewFromUtf8(isolate, cBuf));
  return scope.Escape(e->ToObject());
}

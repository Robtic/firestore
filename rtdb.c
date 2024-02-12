#include <string.h>
#include "types.h"
#include "esp_log.h"
#include "esp_http_client.h"

#include "firestore.h"
#include "config.h"

#define TAG                                      __FILE__

esp_err_t _rtdb_http_event_handler(esp_http_client_event_t *pstEvent);

STATIC firestore_ctx_t stRtdb;

firestore_err_t rtdb_init(void)
{
#ifdef CONFIG_CLOUD_FIRESTORE_LIB_DEBUG
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif /* FIRESTORE_DEBUG */
  
  char host_path[64];
  memset(&host_path,'\0',64);
  // Firebase does not use project ID in the domain, it uses it with &ns=<project_id> in request query
  #ifdef CONFIG_CLOUD_DEV_MODE
  sprintf(host_path,"%s",CONFIG_CLOUD_RTDBS_HOSTNAME);
  #else
  sprintf(host_path,"%s.%s",CONFIG_CLOUD_FIRESTORE_PROJECT_ID,CONFIG_CLOUD_RTDBS_HOSTNAME);
  #endif

  memset(&stRtdb, 0x00, sizeof(stRtdb));
  stRtdb.stHttpconfig.host = CONFIG_CLOUD_RTDBS_HOSTNAME;
  stRtdb.stHttpconfig.port = CONFIG_CLOUD_RTDBS_HOST_PORT;
  stRtdb.stHttpconfig.buffer_size = FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE;
  stRtdb.stHttpconfig.buffer_size_tx = FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE;
  stRtdb.stHttpconfig.event_handler = _rtdb_http_event_handler;
#ifdef CONFIG_CLOUD_FIRESTORE_USE_SSL
  stRtdb.stHttpconfig.cert_pem = FIRESTORE_FIREBASE_CA_CERT_PEM;
  stRtdb.stHttpconfig.transport_type = HTTP_TRANSPORT_OVER_SSL;
#else
  stRtdb.stHttpconfig.transport_type = HTTP_TRANSPORT_OVER_TCP;
#endif
  stRtdb.stHttpconfig.user_data = stRtdb.tcHttpBody;
  return FIRESTORE_OK;
}

firestore_err_t rtdb_put_data(char *path, char *data)
{
    int32_t s32Length;
    firestore_err_t eRetVal;

    #ifdef CONFIG_CLOUD_DEV_MODE
    s32Length = snprintf(stRtdb.tcPath,
                         FIRESTORE_HTTP_PATH_SIZE,
                         "%s.json?ns=%s",
                         path,
                         CONFIG_CLOUD_FIRESTORE_PROJECT_ID);
    #else
    s32Length = snprintf(stRtdb.tcPath,
                         FIRESTORE_HTTP_PATH_SIZE,
                         "%s.json?auth=%s",
                         path,
                         CONFIG_CLOUD_RTDBS_API_KEY);
    #endif
    eRetVal = FIRESTORE_OK;

     if(path != NULL && data != NULL)
    {
        if(s32Length > 0)
        {
            stRtdb.stHttpconfig.path = stRtdb.tcPath;
            ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
            ESP_LOGI(TAG, "HTTP data: %s (%d)", data,strlen(data));
            stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
            esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_PUT);
            esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
            esp_http_client_set_post_field(stRtdb.pstHttpClient, data, strlen(data));
            if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
            {
                stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
                ESP_LOGI(TAG,
                            "HTTP PUT Status = %d, content_length = %lld",
                            esp_http_client_get_status_code(stRtdb.pstHttpClient),
                            esp_http_client_get_content_length(stRtdb.pstHttpClient));
                if(200 != stRtdb.s16LastHttpCode)
                {
                    ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
                    eRetVal = FIRESTORE_ERR_HTTP;
                }
            }
            else
            {
                eRetVal = FIRESTORE_ERR_HTTP;
            }
        }
        else
        {
            eRetVal = FIRESTORE_ERR;
        }
    }
    else
    {
        eRetVal = FIRESTORE_ERR_ARG;
    }
    stRtdb.u32HttpBodyLen = 0;
    esp_http_client_cleanup(stRtdb.pstHttpClient);
    return eRetVal;
}

firestore_err_t rtdb_post_data(char *path, char *data)
{
    int32_t s32Length;
    firestore_err_t eRetVal;
    #ifdef CONFIG_CLOUD_DEV_MODE
    s32Length = snprintf(stRtdb.tcPath,
                            FIRESTORE_HTTP_PATH_SIZE,
                            "%s.json?ns=%s",
                            path,
                            CONFIG_CLOUD_FIRESTORE_PROJECT_ID);
    #else
    s32Length = snprintf(stRtdb.tcPath,
                            FIRESTORE_HTTP_PATH_SIZE,
                            "%s.json?auth=%s",
                            path,
                            CONFIG_CLOUD_RTDBS_API_KEY);
    #endif
    eRetVal = FIRESTORE_OK;
  if(path != NULL && data != NULL)
  {
    if(s32Length > 0)
    {
      stRtdb.stHttpconfig.path = stRtdb.tcPath;
      ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
      ESP_LOGI(TAG, "HTTP data: %s (%d)", data,strlen(data));
      stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
      esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_POST);
      esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
      esp_http_client_set_post_field(stRtdb.pstHttpClient, data, strlen(data));
      if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
      {
        stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
        ESP_LOGI(TAG,
                 "HTTP POST Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(stRtdb.pstHttpClient),
                 esp_http_client_get_content_length(stRtdb.pstHttpClient));
        if(200 != stRtdb.s16LastHttpCode)
        {
          ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
          eRetVal = FIRESTORE_ERR_HTTP;
        }
      }
      else
      {
        eRetVal = FIRESTORE_ERR_HTTP;
      }
    }
    else
    {
      eRetVal = FIRESTORE_ERR;
    }
  }
  else
  {
    eRetVal = FIRESTORE_ERR_ARG;
  }
  stRtdb.u32HttpBodyLen = 0;
  esp_http_client_cleanup(stRtdb.pstHttpClient);
  return eRetVal;
}

firestore_err_t rtdb_get_path(char *path,char **ppcDocs,uint32_t *pu32DocsLen)
{

  int32_t s32Length;
  firestore_err_t eRetVal;

  eRetVal = FIRESTORE_OK;
  if(path)
  {
    #ifdef CONFIG_CLOUD_DEV_MODE
    s32Length = snprintf(stRtdb.tcPath,
                            FIRESTORE_HTTP_PATH_SIZE,
                            "%s.json?ns=%s",
                            path,
                            CONFIG_CLOUD_FIRESTORE_PROJECT_ID);
    #else
    s32Length = snprintf(stRtdb.tcPath,
                            FIRESTORE_HTTP_PATH_SIZE,
                            "%s.json?auth=%s",
                            path,
                            CONFIG_CLOUD_RTDBS_API_KEY);
    #endif
    if(s32Length > 0)
    {
      stRtdb.stHttpconfig.path = stRtdb.tcPath;
      ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
      stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
      esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_GET);
      if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
      {
        stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
        ESP_LOGI(TAG,
                 "HTTP code: %d, content_length: %lld",
                 stRtdb.s16LastHttpCode,
                 esp_http_client_get_content_length(stRtdb.pstHttpClient));
        if(200 != stRtdb.s16LastHttpCode)
        {
          ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
          eRetVal = FIRESTORE_ERR_HTTP;
        }
        else
        {
          *ppcDocs = stRtdb.tcHttpBody;
          *pu32DocsLen = stRtdb.u32HttpBodyLen;
        }
      }
      else
      {
        eRetVal = FIRESTORE_ERR_HTTP;
      }
    }
    else
    {
      eRetVal = FIRESTORE_ERR;
    }
  }
  else
  {
    eRetVal = FIRESTORE_ERR_ARG;
  }
  stRtdb.u32HttpBodyLen = 0;
  esp_http_client_cleanup(stRtdb.pstHttpClient);
  return eRetVal;
}


// firestore_err_t firestore_get_document(char *pcCollectionId,
//                                        char *pcDocumentId,
//                                        char **ppcDocument,
//                                        uint32_t *pu32DocumentLen)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcCollectionId && pcDocumentId)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          pcCollectionId,
//                          pcDocumentId,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP code: %d, content_length: %lld",
//                  stRtdb.s16LastHttpCode,
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           *ppcDocument = stRtdb.tcHttpBody;
//           *pu32DocumentLen = stRtdb.u32HttpBodyLen;
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

// firestore_err_t firestore_add_document(char *pcCollectionId,
//                                        char *pcDocumentId,
//                                        char *pcDocument,
//                                        uint32_t *pu32DocumentLen)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcCollectionId && pcDocumentId)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents/%s?documentId=%s&key=%s",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          pcCollectionId,
//                          pcDocumentId,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_POST);
//       esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
//       esp_http_client_set_post_field(stRtdb.pstHttpClient, pcDocument, strlen(pcDocument));
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP PATCH Status = %d, content_length = %lld",
//                  esp_http_client_get_status_code(stRtdb.pstHttpClient),
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           pcDocument = stRtdb.tcHttpBody;
//           *pu32DocumentLen = stRtdb.u32HttpBodyLen;
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

// firestore_err_t firestore_add_document_autogen(char *pcCollectionId,
//                                        char *pcDocument,
//                                        uint32_t *pu32DocumentLen)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcCollectionId)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents/%s?key=%s",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          pcCollectionId,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_POST);
//       esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
//       esp_http_client_set_post_field(stRtdb.pstHttpClient, pcDocument, strlen(pcDocument));
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP PATCH Status = %d, content_length = %lld",
//                  esp_http_client_get_status_code(stRtdb.pstHttpClient),
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           pcDocument = stRtdb.tcHttpBody;
//           *pu32DocumentLen = stRtdb.u32HttpBodyLen;
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

// firestore_err_t firestore_update_document(char *pcCollectionId,
//                                           char *pcDocumentId,
//                                           char *pcDocument,
//                                           uint32_t *pu32DocumentLen)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcCollectionId && pcDocumentId)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          pcCollectionId,
//                          pcDocumentId,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_PATCH);
//       esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
//       esp_http_client_set_post_field(stRtdb.pstHttpClient, pcDocument, strlen(pcDocument));
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP PATCH Status = %d, content_length = %lld",
//                  esp_http_client_get_status_code(stRtdb.pstHttpClient),
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           pcDocument = stRtdb.tcHttpBody;
//           *pu32DocumentLen = stRtdb.u32HttpBodyLen;
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

// firestore_err_t firestore_ack_action_list(char *pcCollectionId,
//                                           char *pcDocumentId,
//                                           char *pcDocument,
//                                           uint32_t *pu32DocumentLen)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcCollectionId && pcDocumentId)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s&updateMask.fieldPaths=acked",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          pcCollectionId,
//                          pcDocumentId,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_PATCH);
//       esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
//       esp_http_client_set_post_field(stRtdb.pstHttpClient, pcDocument, strlen(pcDocument));
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP PATCH Status = %d, content_length = %lld",
//                  esp_http_client_get_status_code(stRtdb.pstHttpClient),
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           pcDocument = stRtdb.tcHttpBody;
//           *pu32DocumentLen = stRtdb.u32HttpBodyLen;
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

// firestore_err_t firestore_delete_document(char *pcCollectionId, char *pcDocumentId)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcCollectionId && pcDocumentId)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents/%s/%s?key=%s",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          pcCollectionId,
//                          pcDocumentId,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_DELETE);
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP code: %d, content_length: %lld",
//                  stRtdb.s16LastHttpCode,
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           ESP_LOGI(TAG, "Document deleted successfully");
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

// int16_t rtdb_get_last_http_err()
// {
//   return stRtdb.s16LastHttpCode;
// }

// firestore_err_t firestore_run_query(char **ppcDocument,
//                                     char *pcDocument,
//                                     uint32_t *pu32DocumentLen)
// {
  
//   int32_t s32Length;
//   firestore_err_t eRetVal;

//   eRetVal = FIRESTORE_OK;
//   if(pcDocument)
//   {
//     s32Length = snprintf(stRtdb.tcPath,
//                          FIRESTORE_HTTP_PATH_SIZE,
//                          "/v1/projects/%s/databases/(default)/documents:runQuery?key=%s",
//                          CONFIG_CLOUD_FIRESTORE_PROJECT_ID,
//                          CONFIG_CLOUD_FIRESTORE_API_KEY);
//     if(s32Length > 0)
//     {
//       stRtdb.stHttpconfig.path = stRtdb.tcPath;
//       ESP_LOGI(TAG, "HTTP path: %s", stRtdb.stHttpconfig.path);
//       stRtdb.pstHttpClient = esp_http_client_init(&stRtdb.stHttpconfig);
//       esp_http_client_set_method(stRtdb.pstHttpClient, HTTP_METHOD_POST);
//       esp_http_client_set_header(stRtdb.pstHttpClient, "Content-Type", "application/json");
//       esp_http_client_set_post_field(stRtdb.pstHttpClient, pcDocument, strlen(pcDocument));
//       if(ESP_OK == esp_http_client_perform(stRtdb.pstHttpClient))
//       {
//         stRtdb.s16LastHttpCode = esp_http_client_get_status_code(stRtdb.pstHttpClient);
//         ESP_LOGI(TAG,
//                  "HTTP PATCH Status = %d, content_length = %lld",
//                  esp_http_client_get_status_code(stRtdb.pstHttpClient),
//                  esp_http_client_get_content_length(stRtdb.pstHttpClient));
//         if(200 != stRtdb.s16LastHttpCode)
//         {
//           ESP_LOGE(TAG, "Firestore REST API call failed with HTTP code: %d", stRtdb.s16LastHttpCode);
//           eRetVal = FIRESTORE_ERR_HTTP;
//         }
//         else
//         {
//           *ppcDocument = stRtdb.tcHttpBody;
//           *pu32DocumentLen = stRtdb.u32HttpBodyLen;
//         }
//       }
//       else
//       {
//         eRetVal = FIRESTORE_ERR_HTTP;
//       }
//     }
//     else
//     {
//       eRetVal = FIRESTORE_ERR;
//     }
//   }
//   else
//   {
//     eRetVal = FIRESTORE_ERR_ARG;
//   }
//   stRtdb.u32HttpBodyLen = 0;
//   esp_http_client_cleanup(stRtdb.pstHttpClient);
//   return eRetVal;
// }

esp_err_t _rtdb_http_event_handler(esp_http_client_event_t *pstEvent)
{
  esp_err_t s32RetVal;

  s32RetVal = ESP_OK;
  switch(pstEvent->event_id)
  {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(TAG, "HTTP error");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG, "HTTP connected to server");
    break;
  case HTTP_EVENT_HEADERS_SENT:
    ESP_LOGD(TAG, "All HTTP headers are sent to server");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "HTTP header received: \t%s:%s",pstEvent->header_key,pstEvent->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    /* If user_data buffer is configured, copy the response into it */
    if(pstEvent->user_data)
    {
      strncpy(pstEvent->user_data+stRtdb.u32HttpBodyLen,
              (char*)pstEvent->data,
              pstEvent->data_len);
      stRtdb.u32HttpBodyLen += pstEvent->data_len;
    }
    /* Else you can copy the response into a global HTTP buffer */
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP session is finished");
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGD(TAG, "HTTP connection is closed");
    break;
  default:
    ESP_LOGD(TAG, "HTTP Event Redirected");
  }
  return s32RetVal;
}

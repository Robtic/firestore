#ifndef __FIRESTORE_H__
#define __FIRESTORE_H__

#include <stdint.h>
#include "esp_http_client.h"
#include "config.h"

typedef enum
{
  FIRESTORE_OK                = 0,               /**< No error, everything OK  */
  FIRESTORE_ERR               = -1,              /**< Generic error            */
  FIRESTORE_ERR_HTTP          = -2,              /**< Timeout                  */
  FIRESTORE_ERR_ARG           = -4,              /**< Illegal argument         */
  FIRESTORE_ERR_UNHANDLED     = -5,              /**< Unhandled feature/option */
  FIRESTORE_ERR_MCU           = -6,              /**< MCU error                */
  FIRESTORE_ERR_OS            = -7,              /**< Operating system error   */
}firestore_err_t;


typedef struct
{
  char tcPath[FIRESTORE_HTTP_PATH_SIZE];
  uint32_t u32PathLen;
  char tcHttpBody[FIRESTORE_HTTP_RX_BUF_SIZE];
  uint32_t u32HttpBodyLen;
  esp_http_client_handle_t pstHttpClient;
  esp_http_client_config_t stHttpconfig;
  int16_t s16LastHttpCode;
}firestore_ctx_t;

firestore_err_t firestore_init(void);
firestore_err_t firestore_get_collection(char *pcCollectionId,
                                         char **pcDocs,
                                         uint32_t *pu32DocsLen);
firestore_err_t firestore_get_document(char *pcCollectionId,
                                       char *pcDocumentId,
                                       char **ppcDocument,
                                       uint32_t *pu32DocumentLen);
firestore_err_t firestore_add_document(char *pcCollectionId,
                                       char *pcDocumentId,
                                       char *pcDocument,
                                       uint32_t *pu32DocumentLen);
firestore_err_t firestore_add_document_autogen(char *pcCollectionId,
                                       char *pcDocument,
                                       uint32_t *pu32DocumentLen);
firestore_err_t firestore_update_document(char *pcCollectionId,
                                          char *pcDocumentId,
                                          char *pcDocument,
                                          uint32_t *pu32DocumentLen);
firestore_err_t firestore_ack_action_list(char *pcCollectionId,
                                          char *pcDocumentId,
                                          char *pcDocument,
                                          uint32_t *pu32DocumentLen);
firestore_err_t firestore_delete_document(char *pcCollectionId,
                                          char *pcDocumentId);
firestore_err_t firestore_run_query(char **ppcDocument,
                                    char *pcDocument,
                                    uint32_t *pu32DocumentLen);
int16_t firestore_get_last_http_err();

#endif /* __FIRESTORE_H__ */
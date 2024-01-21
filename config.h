#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifndef FIRESTORE_HOSTNAME
#define FIRESTORE_HOSTNAME                       "firestore.googleapis.com"
#endif /* FIRESTORE_HOSTNAME */

#ifndef FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE
#define FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE      1024
#endif /* FIRESTORE_HTTP_INTERNAL_TX_BUF_SIZE */

#ifndef FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE
#define FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE      1024
#endif /* FIRESTORE_HTTP_INTERNAL_RX_BUF_SIZE */

#ifndef FIRESTORE_HTTP_RX_BUF_SIZE
#define FIRESTORE_HTTP_RX_BUF_SIZE               1024*2
#endif /* FIRESTORE_HTTP_RX_BUF_SIZE */

#ifndef FIRESTORE_HTTP_PATH_SIZE
#define FIRESTORE_HTTP_PATH_SIZE                 256
#endif /* FIRESTORE_HTTP_PATH_SIZE */

#ifndef FIRESTORE_FIREBASE_PROJECT_ID
#define FIRESTORE_FIREBASE_PROJECT_ID            CONFIG_CLOUD_FIRESTORE_PROJECT_ID
#endif /* FIRESTORE_FIREBASE_PROJECT_ID */

#ifndef FIRESTORE_FIREBASE_API_KEY
#define FIRESTORE_FIREBASE_API_KEY               CONFIG_CLOUD_FIRESTORE_API_KEY
#endif /* FIRESTORE_FIREBASE_API_KEY */

#ifndef FIRESTORE_FIREBASE_CA_CERT_PEM
/* openssl s_client -showcerts -verify 5 -connect edgecert.googleapis.com:443 < /dev/null */
#define FIRESTORE_FIREBASE_CA_CERT_PEM                                                              \
                                          "-----BEGIN CERTIFICATE-----\r\n"                         \
                            "MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX\r\n"  \
                            "MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\r\n"  \
                            "CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx\r\n"  \
                            "OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\r\n"  \
                            "GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx\r\n"  \
                            "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63\r\n"  \
                            "ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS\r\n"  \
                            "iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k\r\n"  \
                            "KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ\r\n"  \
                            "DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk\r\n"  \
                            "j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5\r\n"  \
                            "cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW\r\n"  \
                            "CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499\r\n"  \
                            "iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei\r\n"  \
                            "Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap\r\n"  \
                            "sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b\r\n"  \
                            "9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP\r\n"  \
                            "BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf\r\n"  \
                            "BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw\r\n"  \
                            "JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH\r\n"  \
                            "MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al\r\n"  \
                            "oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy\r\n"  \
                            "MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF\r\n"  \
                            "AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9\r\n"  \
                            "NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9\r\n"  \
                            "WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw\r\n"  \
                            "9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy\r\n"  \
                            "+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi\r\n"  \
                            "d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=\r\n"              \
                                            "-----END CERTIFICATE-----\r\n"

#endif /* FIRESTORE_FIREBASE_CA_CERT_PEM */

#endif /* __CONFIG_H__ */
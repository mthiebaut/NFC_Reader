#include <stdlib.h>
#include <nfc/nfc.h>

static void print_hex(const uint8_t *pbtData, const size_t szBytes)
{
    size_t  szPos;

    for (szPos = 0; szPos < szBytes; szPos++)
    {
        printf("%02x  ", pbtData[szPos]);
    }

    printf("\n");
}

int main(int argc, const char *argv[])
{
    nfc_device *pnd;
    nfc_target nt;
    nfc_context *context;

    // Init
    nfc_init(&context);

    if (context == NULL)
    {
        printf("Unable to init libnfc (malloc)\n");
        exit(EXIT_FAILURE);
    }
    const char *acLibnfcVersion = nfc_version();

    pnd = nfc_open(context, NULL);

    if (pnd == NULL)
    {
        printf("ERROR: %s\n", "Unable to open NFC device.");
        exit(EXIT_FAILURE);
    }

    if (nfc_initiator_init(pnd) < 0)
    {
        nfc_perror(pnd, "nfc_initiator_init");
        exit(EXIT_FAILURE);
    }

    printf("Lecteur NFC : %s ouvert\n", nfc_device_get_name(pnd));
    const nfc_modulation nmMifare =
    {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
    };

    if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0)
    {
        printf("L'étiquette (NFC) ISO14443A suivante a été trouvée :\n\n");
	//ATQA
        printf("    ATQA (SENS_RES): ");
        printf("\033[%sm","31");
        print_hex(nt.nti.nai.abtAtqa, 2);
        printf("\033[%sm","0");
	// UID
        printf("       UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
        printf("\033[%sm","31");
        print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
        printf("\033[%sm","0");
	// SAK
        printf("      SAK (SEL_RES): ");
        printf("\033[%sm","31");
        print_hex(&nt.nti.nai.btSak, 1);
        printf("\033[%sm","0");

        if (nt.nti.nai.szAtsLen)
        {
	    // ATS
            printf("          ATS (ATR): ");
            printf("\033[%sm","31");
            print_hex(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
            printf("\033[%sm","0");
        }
        printf("\n");
    }

    // Close & Exit
    nfc_close(pnd);
    nfc_exit(context);
    exit(EXIT_SUCCESS);
}

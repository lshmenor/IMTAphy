# This defines the recipients for the Bazaar post push hook. It is
# expected to be found in the ./config directory within the branch
# that you push
#
# Each entry in the recipients list is a pair of regular expression
# and space separated email address string. The regular expression
# is matched against the location that you push to. If it matches than
# emails are sent to the recipients.

recipients = [(".*@bazaar.launchpad.net/~comnets.*",
               "develop@openwns.org"),
	      (".*bazaar.comnets.rwth-aachen.de/openWNS.*",
               "software@comnets.rwth-aachen.de"),
              (".*bazaar.comnets.rwth-aachen.de/RS/interference.*",
               "rs@comnets.rwth-aachen.de kks@comnets.rwth-aachen.de"),
              (".*bazaar.comnets.rwth-aachen.de/RS/scheduling.*",
               "rs@comnets.rwth-aachen.de aoz@comnets.rwth-aachen.de fei@comnets.rwth-aachen.de"),
              (".*@bazaar-extern.comnets.rwth-aachen.de:1122/home/scponly/bazaar/openWNS.*",
               "friendly-user@openwns.org"),
              (".*@bazaar-extern.comnets.rwth-aachen.de:1122/~/.*",
               "mue@comnets.rwth-aachen.de dbn@comnets.rwth-aachen.de msg@comnets.rwth-aachen.de"),
              (".*@bazaar.comnets.rwth-aachen.de/KKS.*",
               "kks@comnets.rwth-aachen.de"),
              (".*bazaar.comnets.rwth-aachen.de/AOZ.*",
               "aoz@comnets.rwth-aachen.de rs@comnets.rwth-aachen.de dbn@comnets.rwth-aachen.de"),
              (".*bazaar.comnets.rwth-aachen.de/DBN/openWNS/reorder.*",
               "dbn@comnets.rwth-aachen.de rs@comnets.rwth-aachen.de mue@comnets.rwth-aachen.de fei@comnets.rwth-aachen.de bmw@comnets.rwth-aachen.de")]

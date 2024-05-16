// include section
#include "../../common/common.h"
#include "stdlib.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <string.h>
#include <math.h>
#include <inttypes.h>

// define section
#define APP_NAME_ADC "[ADC] "

#define SAMPLES 1024
#define FINAL_SIZE (SAMPLES / 2)

#define ATTENUATION ADC_ATTEN_DB_12
#define WIDTH ADC_WIDTH_BIT_DEFAULT

// global vars
static esp_adc_cal_characteristics_t adc1_chars;

float* array;
float duration = 1.0;
int n = SAMPLES;

// variables for z-score calculation
float max = 0.0;
float mean = 0.0;
float std_dev = 0.0;
float threshold = 5.0;

char example_wave[] = "1.3768585623545084, 2.7314645434828586, 4.042024650445045, 5.287653916202229, 6.448804471677176, 7.507664503461697, 8.448518533902547, 9.258061049311815, 9.92565657385212, 10.443540516296071, 10.806956475741755, 11.01422714887282, 11.066757501616433, 10.96897041674291, 10.728176570095336, 10.35438178596618, 9.860036541917754, 9.259733602191073, 8.569860926495995, 7.80821800048124, 6.99360454255276, 6.145391140438122, 5.283081746393212, 4.425878103858279, 3.592256087758503, 2.7995636180631798, 2.0636492596442197, 1.3985298641189106, 0.8161046593523844, 0.32592207225024605, -0.06499569303415154, -0.3522666764172371, -0.5341995957609136, -0.6117800110768279, -0.5885930527730054, -0.4706850391527584, -0.26636774039154876, 0.014029614835675552, 0.3584542793839751, 0.7534625019272919, 1.1845823589767013, 1.636697962081422, 2.094445625615913, 2.542612333260825, 2.9665268242198963, 3.352433833414724, 3.6878424579658406, 3.9618402744968035, 4.165365682433515, 4.291431977083942, 4.335297838252286, 4.294580227236409, 4.169307086106957, 3.961908694795364, 3.6771480289340484, 3.321991939209538, 2.9054264060624613, 2.4382204778225542, 1.9326447436020773, 1.4021512949224684, 0.8610230658832487, 0.3240011884203104, -0.19410046096087985, -0.6787840265556295, -1.1162501619779293, -1.4937647901504207, -1.800001047056073, -2.02534790369233, -2.162177747335768, -2.2050661758891694, -2.150958394810687, -1.9992778781078067, -1.7519743330657453, -1.4135094601828633, -0.9907804908474187, -0.49298298029499543, 0.0685842031091961, 0.6807673539489647, 1.328854235053904, 1.9969114083796349, 2.668154778536173, 3.3253447090738875, 3.951196446910857, 4.528796182930025, 5.042012896827872, 5.475896186453588, 5.8170505653766185, 6.053977221053388, 6.177374948221705, 6.180392891486218, 6.058828826271119, 5.811267953049738, 5.4391585470899235, 4.946822263065741, 4.341398406819054, 3.6327230199977905, 2.833145141416367, 1.9572840762576975, 1.0217328862959054, 0.044714578748517475, -0.9543014115328505, -1.9550139083390357, -2.936726563037688, -3.8787964863105406, -4.761084815090911, -5.56439901310467, -6.270916865182423, -6.8645825195416235, -7.331465549618644, -7.6600748336369175, -7.8416200669112985, -7.870214905455841, -7.7430170624263335, -7.460302110679667, -7.025469252112686, -6.444978862528361, -5.728223173687974, -4.887332975948741, -3.9369246801864355, -2.8937934328416746, -1.7765592015185392, -0.6052738122922858, 0.5990022008414784, 1.8146433993986202, 3.0199139074385144, 4.193422329217654, 5.314566916390097, 6.363961030678926, 7.323829424061019, 8.178366552424045, 8.914049033986444, 9.519895439446532, 9.98766783158777, 10.312010829060856, 10.49052542051683, 10.52377626723517, 10.415232769555068, 10.171145698905852, 9.800362677464468, 9.314087186987749, 8.72558707465034, 8.049859666929473, 7.303261576270774, 6.503112067001351, 5.667279418815684, 4.8137600751781005, 3.960260482436788, 3.1237914110299867, 2.320284206049704, 1.5642378491778643, 0.8684049413672641, 0.24352375421573225, -0.3018976292023421, -0.7617403293279548, -1.1323734609872975, -1.4126907710346366, -1.6040846861589588, -1.7103588330355919, -1.7375815626231095, -1.693884416109747, -1.5892107786044731, -1.4350211472701244, -1.2439624653891679, -1.0295098187165843, -0.8055894355685503, -0.5861923623095673, -0.38498839122624195, -0.2149497935227629, -0.0879941570951086, -0.01465515300208775, -0.003789367588771242, -0.06232645546696425, -0.19506881305995627, -0.4045457683340983, -0.6909259583388305, -1.0519901537765621, -1.4831653226332024, -1.9776192378341255, -2.526413462340319, -3.118711124100265, -3.7420345566756064, -4.382566661027463, -5.025488769016302, -5.655346885333179, -6.256437473513472, -6.813203450510411, -7.310630775190936, -7.734635965991461, -8.07243506334947, -8.31288495948332, -8.44678864233368, -8.467156727579006, -8.369418663352999, -8.151578163033209, -7.814308724838238, -7.360986502408771, -6.797659265053791, -6.132951695246959, -5.377908778735071, -4.545780513727861, -3.6517525653499336, -2.712628786760149, -1.7464726884027202, -0.7722159342086364, 0.19075724554172016, 1.123040229957131, 2.0056388925797086, 2.820401161794561, 3.550431302000737, 4.1804792728456075, 4.697296061435957, 5.089946633602002, 5.350073097834585, 5.4721017976574196, 5.453389319047631, 5.2943037896822505, 4.998239324056712, 4.5715629986153035, 4.0234952884128505, 3.3659264255157693, 2.6131726137827043, 1.7816774205121546, 0.8896649304235738, -0.043247637937618766, -0.99646921500667, -1.948876683830063, -2.8792693016447455, -3.7668255398549286, -4.591552080955823, -5.334714885334902, -5.9792426475379985, -6.510093591112177, -6.9145773889235365, -7.182625022448342, -7.3070005851027755, -7.2834503635280665, -7.110785966012785, -6.7908997755710665, -6.328712551576726, -5.732054552380741, -5.011483066083331, -4.1800406824733525, -3.2529599825941338, -2.247321532370065, -1.1816731152852533, -0.07561900209396108, 1.0506112868435364, 2.176604833758145, 3.282207690308987, 4.3479576867624115, 5.355497872412709, 6.287961264450228, 7.130318286093357, 7.8696791768978365, 8.495544737292164, 8.999999999999996, 9.377846774020295, 9.626672450336528, 9.746853958527716, 9.741497284689178, 9.616314467817542, 9.379441448740685, 9.041201518659364, 8.613820371315043, 8.111099874372913, 7.548058615939458, 6.940548029401179, 6.304853436738076, 5.657289664859101, 5.013800974321369, 4.389574893434194, 3.7986791770796207, 3.2537305178208094, 2.765602841375907, 2.343182038463314, 1.99317284386622, 1.719962298603206, 1.5255428527449117, 1.4094967175267543, 1.36904159045862, 1.3991363914425574, 1.4926441967916637, 1.6405481759746623, 1.832215055720369, 2.05569948820606, 2.298081711687038, 2.545830086578087, 2.7851794868085786, 3.0025161395929656, 3.184759345802976, 3.319730581772961, 3.3965007800460087, 3.405707104311311, 3.3398312603810654, 3.193432303353876, 2.9633279894000513, 2.648719953144569, 2.251259339238148, 1.7750509474825926, 1.2265954309051277, 0.6146705803603512, -0.049845797886458154, -0.7542074885627175, -1.4840807081602927, -2.223867725458892, -2.957065752536646, -3.6666527179438035, -4.3354908513700945, -4.946738540007803, -5.484260672402322, -5.933027672097737, -6.279493641607328, -6.511944482488481, -6.620807519544657, -6.59891502126693, -6.441715054640696, -6.147424316159172, -5.717118914341543, -5.154760511204661, -4.467156727579008, -3.663856244912383, -2.756980558533609, -1.7609958185990326, -0.692429600381955, 0.4304612578118756, 1.5880644535055273, 2.7598193693514417, 3.924648485772135, 5.061403378039709, 6.149315250171071, 7.168439821292975, 8.100086478918875, 8.92722194588951, 9.634839266123798, 10.210283686083322, 10.643527976268583, 10.927390877852869, 11.05769364738551, 11.033351077579777, 10.85639486201675, 10.531928711632917, 10.06801618541936, 9.475503730805599, 8.76778290517982, 7.960497134689234, 7.071199627769817, 6.118970169545846, 5.123999453655084, 4.107150338672471, 3.0895059302787287, 2.091914675837874, 1.1345427086364617, 0.23644349368868856, -0.5848455897765699, -1.3136707323795465, -1.936585093300783, -2.442620993272765, -2.8235049755150436, -3.0738111368401118, -3.1910495631982454, -3.1756882059862344, -3.031108072469619, -2.7634931398179927, -2.3816579017563115, -1.8968168848024578, -1.3223017943266275, -0.6732331385419126, 0.03384579646171382, 0.7813639090331536, 1.551136837272495, 2.3247913503882534, 3.084189635915317, 3.8118435925772243, 4.49130952009855, 5.1075541032806395, 5.647283305807315, 6.099226702248869, 6.454370862725492, 6.706136637776648, 6.850496541844053, 6.886029870349403, 6.813914673604691, 6.637857215516183, 6.363961030678937, 6.000539124934923, 5.557874208037001, 5.047933071075554, 4.484042297001107, 3.8805333946525806, 3.252366154043081, 2.6147395167494167, 1.9826995285862057, 1.3707539860879634, 0.7925032028585601, 0.2602959111992744, -0.21508131145590603, -0.6246735278505744, -0.9615792810227011, -1.2211193951887096, -1.4009486349161975, -1.5011075095536741, -1.524012964132194, -1.4743881823793865, -1.3591332067553195, -1.1871395193147163, -0.9690530914571682, -0.7169916677374046, -0.44422316889655433, -0.16481305539991042, 0.10675073784804373, 0.35594612075263576, 0.5686089010146782, 0.7313189325164964, 0.8317708329986568, 0.8591200851045935, 0.8042958952515935, 0.6602729523767148, 0.42229518551687306, 0.08804574302829682, -0.34224131932193025, -0.8657307788432753, -1.4769990985931871, -2.1681105776937706, -2.928755477649182, -3.746447322524071, -4.606775151967855, -5.493705182393835, -6.389925134553664, -7.277223443728888, -8.136894706645768, -8.95016205765552, -9.69860672166553, -10.364594773662608, -10.931691150008966, -11.385051205120645, -11.711780583314123, -11.901254868816025, -11.945391371381387, -11.838866480121762, -11.579273249213106, -11.16721523767563, -10.606334079905112, -9.903269780338587, -9.067554269301844, -8.111440291806533, -7.049669191100161, -5.899182559399252, -4.678784026555608, -3.408758613109224, -2.1104580602713563, -0.8058613427958436, 0.4828798471543796, 1.7339004998152308, 2.9260763021202125, 4.039462109815864, 5.0557050670890105, 5.958423135681821, 6.733540580437543, 7.369572937712029, 7.857855144397865, 8.1927078017378, 8.371537959577706, 8.394872300472125, 8.266322144451664, 7.992481248529295, 7.582758904146772, 7.049152305295527, 6.405963535957692, 5.669467775967881, 4.85754042050071, 3.9892517248797423, 3.084438302209518, 2.1632613000867993, 1.2457613530698946, 0.35142044371001724, -0.5012593936829837, -1.2951510207632273, -2.0148619363676192, -2.6470508049303008, -3.1806938011634056, -3.607294994618898, -3.921036323266855, -4.118864130498414, -4.200510731456383, -4.168450997286271, -4.027795464063368, -3.7861229512220076, -3.453257077712518, -3.04099236005705, -2.562776734755155, -2.03335834115322, -1.468405206913016, -0.8841070779801199, -0.29676901475588857, 0.2775934725202509, 0.8236480565058146, 1.3271186105778194, 1.7751287599481698, 2.1565078541632006, 2.4620521797646733, 2.6847353310553186, 2.819862898262664, 2.865167986322744, 2.8208455117549285, 2.6895247055521634, 2.4761807414834465, 2.187987876272315, 1.8341178959632352, 1.4254889780106672, 0.9744712700441429, 0.49455652562637553, -7.843929320908945e-15, -0.4945565256263912, -0.974471270044158, -1.4254889780106819, -1.8341178959632485, -2.1879878762723273, -2.476180741483457, -2.6895247055521727, -2.8208455117549374, -2.865167986322751, -2.8198628982626683, -2.684735331055321, -2.4620521797646737, -2.1565078541632, -1.7751287599481667, -1.3271186105778146, -0.823648056505808, -0.27759347252024247, 0.2967690147558981, 0.884107077980131, 1.4684052069130291, 2.033358341153234, 2.562776734755169, 3.0409923600570643, 3.4532570777125335, 3.786122951222023, 4.027795464063384, 4.168450997286287, 4.200510731456399, 4.118864130498428, 3.9210363232668692, 3.607294994618911, 3.180693801163417, 2.6470508049303123, 2.0148619363676286, 1.2951510207632362, 0.5012593936829908, -0.3514204437100119, -1.2457613530698897, -2.1632613000867966, -3.0844383022095165, -3.989251724879742, -4.857540420500713, -5.669467775967885, -6.405963535957696, -7.049152305295532, -7.582758904146777, -7.992481248529302, -8.266322144451673, -8.394872300472134, -8.371537959577717, -8.19270780173781, -7.8578551443978775, -7.369572937712041, -6.733540580437555, -5.958423135681835, -5.055705067089026, -4.039462109815879, -2.926076302120228, -1.7339004998152463, -0.482879847154396, 0.805861342795827, 2.11045806027134, 3.408758613109207, 4.67878402655559, 5.899182559399235, 7.0496691911001435, 8.111440291806515, 9.067554269301828, 9.90326978033858, 10.60633407990509, 11.167215237675617, 11.579273249213099, 11.838866480121741, 11.945391371381373, 11.901254868816018, 11.711780583314106, 11.385051205120636, 10.93169115000896, 10.364594773662596, 9.698606721665525, 8.950162057655517, 8.136894706645764, 7.277223443728888, 6.389925134553665, 5.493705182393841, 4.606775151967861, 3.746447322524079, 2.92875547764919, 2.168110577693785, 1.4769990985932013, 0.8657307788432886, 0.34224131932195245, -0.08804574302827817, -0.4222951855168571, -0.660272952376701, -0.80429589525157, -0.8591200851045513, -0.8317708329986528, -0.7313189325164702, -0.5686089010146198, -0.3559461207526018, -0.1067507378480188, 0.16481305539989943, 0.44422316889658653, 0.7169916677374597, 0.9690530914571895, 1.187139519314726, 1.3591332067553137, 1.4743881823794025, 1.5240129641322104, 1.5011075095536879, 1.400948634916206, 1.2211193951887038, 0.9615792810227157, 0.6246735278505762, 0.21508131145591314, -0.2602959111992664, -0.7925032028585886, -1.370753986087979, -1.982699528586203, -2.614739516749392, -3.252366154043091, -3.8805333946526175, -4.484042297001082, -5.047933071075565, -5.5578742080370525, -6.000539124934935, -6.36396103067895, -6.63785721551618, -6.813914673604701, -6.886029870349426, -6.850496541844069, -6.706136637776652, -6.454370862725504, -6.099226702248879, -5.647283305807301, -5.1075541032806635, -4.491309520098554, -3.8118435925771945, -3.084189635915324, -2.3247913503882147, -1.551136837272526, -0.7813639090331496, -0.03384579646166824, 0.6732331385419115, 1.322301794326635, 1.896816884802437, 2.3816579017563395, 2.763493139818009, 3.031108072469611, 3.175688205986271, 3.1910495631982365, 3.0738111368401224, 2.823504975515059, 2.442620993272804, 1.9365850933007644, 1.3136707323795456, 0.5848455897765699, -0.23644349368864903, -1.134542708636416, -2.091914675837877, -3.089505930278726, -4.10715033867246, -5.123999453655088, -6.118970169545877, -7.071199627769787, -7.960497134689297, -8.76778290517979, -9.475503730805599, -10.068016185419353, -10.531928711632917, -10.856394862016753, -11.033351077579752, -11.057693647385529, -10.927390877852865, -10.643527976268587, -10.210283686083338, -9.63483926612378, -8.927221945889508, -8.100086478918865, -7.168439821293027, -6.149315250171071, -5.061403378039691, -3.924648485772164, -2.759819369351432, -1.5880644535055803, -0.4304612578119009, 0.6924296003819455, 1.7609958185990104, 2.756980558533593, 3.663856244912372, 4.467156727578968, 5.154760511204667, 5.717118914341524, 6.147424316159144, 6.441715054640696, 6.598915021266926, 6.620807519544629, 6.511944482488465, 6.279493641607322, 5.933027672097692, 5.4842606724023355, 4.946738540007806, 4.335490851370055, 3.6666527179437995, 2.9570657525366024, 2.223867725458925, 1.4840807081602962, 0.7542074885626908, 0.049845797886465704, -0.6146705803603556, -1.2265954309051041, -1.7750509474825766, -2.2512593392381635, -2.648719953144554, -2.9633279894000353, -3.1934323033538465, -3.3398312603810583, -3.4057071043112694, -3.3965007800460083, -3.319730581772902, -3.184759345802933, -3.002516139592945, -2.785179486808529, -2.5458300865781225, -2.298081711687031, -2.05569948820599, -1.83221505572036, -1.6405481759746667, -1.4926441967916269, -1.3991363914425232, -1.369041590458572, -1.4094967175267694, -1.5255428527448922, -1.7199622986031846, -1.993172843866228, -2.3431820384633033, -2.7656028413758893, -3.253730517820812, -3.7986791770796358, -4.389574893434171, -5.0138009743213825, -5.657289664859144, -6.304853436738078, -6.940548029401229, -7.548058615939437, -8.111099874372906, -8.613820371315082, -9.041201518659374, -9.379441448740714, -9.61631446781752, -9.741497284689196, -9.746853958527764, -9.626672450336567, -9.377846774020298, -9.000000000000014, -8.495544737292198, -7.86967917689781, -7.130318286093374, -6.287961264450243, -5.355497872412712, -4.347957686762413, -3.2822076903089985, -2.176604833758199, -1.050611286843573, 0.07561900209399708, 1.181673115285213, 2.2473215323700435, 3.2529599825941067, 4.180040682473354, 5.011483066083346, 5.732054552380707, 6.328712551576728, 6.790899775571086, 7.110785966012781, 7.283450363528074, 7.307000585102781, 7.182625022448336, 6.91457738892353, 6.510093591112185, 5.979242647537985, 5.334714885334929, 4.591552080955851, 3.7668255398548816, 2.879269301644782, 1.9488766838300835, 0.9964692150066522, 0.04324763793760833, -0.8896649304235655, -1.7816774205121173, -2.61317261378273, -3.365926425515797, -4.023495288412818, -4.571562998615283, -4.998239324056703, -5.29430378968224, -5.4533893190476235, -5.472101797657407, -5.35007309783458, -5.089946633601987, -4.697296061435953, -4.180479272845606, -3.5504313020007676, -2.820401161794561, -2.005638892579694, -1.1230402299571498, -0.19075724554172324, 0.7722159342085927, 1.7464726884026986, 2.7126287867601673, 3.6517525653499123, 4.545780513727841, 5.377908778735106, 6.13295169524695, 6.797659265053772, 7.360986502408724, 7.814308724838238, 8.15157816303319, 8.369418663352976, 8.46715672757902, 8.446788642333694, 8.312884959483307, 8.07243506334948, 7.734635965991485, 7.310630775190937, 6.813203450510352, 6.256437473513382, 5.6553468853332385, 5.025488769016327, 4.382566661027443, 3.742034556675608, 3.11871112410023, 2.52641346234035, 1.977619237834154, 1.4831653226332113, 1.0519901537765577, 0.6909259583388225, 0.4045457683341205, 0.19506881305995805, 0.06232645546696425, 0.0037893675888094336, 0.014655153002110843, 0.08799415709516456, 0.21494979352274823, 0.38498839122626194, 0.5861923623095784, 0.8055894355686164, 1.0295098187166962, 1.2439624653891665, 1.4350211472701286, 1.5892107786045002, 1.6938844161098139, 1.737581562623152, 1.7103588330355715, 1.6040846861589682, 1.412690771034673, 1.1323734609873055, 0.7617403293279708, 0.3018976292023634, -0.24352375421568428, -0.8684049413672614, -1.564237849177868, -2.3202842060497195, -3.123791411029942, -3.960260482436762, -4.813760075178079, -5.667279418815698, -6.503112067001392, -7.30326157627085, -8.049859666929429, -8.725587074650335, -9.31408718698777, -9.80036267746451, -10.171145698905917, -10.41523276955504, -10.523776267235167, -10.49052542051685, -10.312010829060887, -9.987667831587768, -9.51989543944655, -8.914049033986473, -8.17836655242409, -7.323829424061012, -6.363961030678897, -5.314566916390172, -4.193422329217675, -3.0199139074385153, -1.8146433993986288, -0.5990022008414546, 0.6052738122922234, 1.7765592015184914, 2.893793432841637, 3.9369246801864293, 4.887332975948754, 5.728223173688007, 6.444978862528324, 7.025469252112667, 7.460302110679661, 7.743017062426331, 7.870214905455842, 7.841620066911298, 7.660074833636921, 7.331465549618654, 6.864582519541607, 6.2709168651823735, 5.5643990131047225, 4.761084815090948, 3.8787964863105198, 2.936726563037663, 1.9550139083389864, 0.9543014115329376, -0.044714578748495715, -1.0217328862959034, -1.95728407625769, -2.8331451414163538, -3.632723019997762, -4.34139840681903, -4.946822263065711, -5.4391585470899315, -5.811267953049732, -6.058828826271085, -6.180392891486198, -6.177374948221718, -6.053977221053367, -5.817050565376569, -5.475896186453507, -5.042012896827913, -4.528796182930038, -3.951196446910832, -3.3253447090738466, -2.6681547785361053, -1.9969114083796766, -1.3288542350539236, -0.6807673539489598, -0.06858420310916902, 0.49298298029502474, 0.9907804908473716, 1.413509460182861, 1.7519743330657378, 1.9992778781077867, 2.150958394810687, 2.2050661758891863, 2.1621777473357637, 2.0253479036922952, 1.8000010470560701, 1.4937647901503759, 1.1162501619778333, 0.6787840265556333, 0.19410046096089695, -0.324001188420343, -0.861023065883307, -1.402151294922521, -1.932644743602039, -2.438220477822539, -2.9054264060624972, -3.321991939209562, -3.6771480289340768, -3.961908694795353, -4.169307086106942, -4.294580227236413, -4.335297838252281, -4.291431977083937, -4.165365682433535, -3.961840274496808, -3.6878424579658358, -3.3524338334147004, -2.966526824219842, -2.5426123332608705, -2.0944456256159363, -1.6366979620814268, -1.184582358976671, -0.7534625019272316, -0.35845427938387076, -0.014029614835710635, 0.2663677403915443, 0.47068503915279614, 0.5885930527730716, 0.6117800110768612, 0.5341995957609154, 0.35226667641726195, 0.06499569303414354, -0.32592207225024605, -0.8161046593523853, -1.3985298641188626, -2.0636492596442118, -2.7995636180631776, -3.5922560877585132, -4.425878103858307, -5.283081746393165, -6.145391140438088, -6.993604542552746, -7.808218000481277, -8.569860926496052, -9.259733602191012, -9.860036541917736, -10.354381785966178, -10.728176570095354, -10.968970416742957, -11.06675750161649, -11.014227148872811, -10.806956475741778, -10.443540516296101, -9.925656573852107, -9.258061049311792, -8.448518533902599, -7.507664503461747, -6.448804471677162, -5.2876539162022205, -4.042024650445021, -2.731464543482951, -1.376858562354544";

void test_init_array(){
    char* token;
    const char* delimiters = ", ";
    int i = 0;

    token = strtok(example_wave, delimiters);
    while (token != NULL && i < SAMPLES) {
        array[i] = strtof(token, NULL);
        i++;
        token = strtok(NULL, delimiters);
    }

    while (i < SAMPLES) {
        array[i] = 0.0f;
        i++;
    }

}

// initialization of adc1 ch0
void characterize_adc1_ch_0(){
    ESP_LOGI(APP_NAME_ADC, "Initializing ADC_1 CH_0...");

    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1_chars);
    ESP_LOGI(APP_NAME_ADC, "Characterization ADC_1 CH_0 : done");
    
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
    ESP_LOGI(APP_NAME_ADC, "Channel width and configuration done");
}

// calculates standard deviation of the sampled values
float std_dev_calc(){
    float std_dev = 0;

    for(int i = 0; i < n; i++){
        std_dev += pow(array[i] - mean, 2);
    }

    std_dev = sqrt(std_dev / n);

    return std_dev;
}

// gets values from the adc and puts
// them into a buffer passed as argument of the function
void get_values_from_adc(){
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Invalid pointer!");
    else if(!n)
        ESP_LOGE(APP_NAME_ADC, "No samples!");
    else{
        int period = pdMS_TO_TICKS(duration);

        for(int i = 0; i < n; i++){
            array[i] = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
            //ESP_LOGI(APP_NAME_ADC, "Sampled: %f mV at index %d", array[i], i);
         
            mean += array[i];

            if(array[i] > max)
                max = array[i];
            
            vTaskDelay(period);
            // delay to define in order to achieve a different sampling frequency    
        }

        mean = (float) (mean / n);
        std_dev = std_dev_calc();
        printf("Max: %f - Average : %f - Stadnard dev: %f - threshold; %f\n", max, mean, std_dev, threshold);

        ESP_LOGI(APP_NAME_ADC, "Sampling completed");
    }
}

// using z-score, it computes if the value is an outlier
int outlier_calc(float value){
    float z = (value - mean) / std_dev;

    if(z > threshold)
        return 1;
    else
        return 0;
}

// finds and prints all the outliers values of a input array
// TEST PURPOSES ONLY!
void outliers_finder_test(float* array, int size){
    if(!array)
        return;

    printf("Outliers:\n");
    for(int i = 0; i < size; i++){
        if(outlier_calc(array[i]))
            printf("idx: %d - value: %f\n", i, array[i]);
    }
}


int max_outlier_finder(float* array, int size){
    if(!array)
        return -1;

    int max_idx = 0;

    for(int i = 0; i < size; i++){
        if(outlier_calc(array[i]))
            //printf("idx: %d - value: %f\n", i, array[i]);
            max_idx = i;
    }

    return max_idx;
}


// initialization of array and ADC_1 with channel 0
void adc_init(){
    array = (float*) malloc(sizeof(float) * SAMPLES);
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Array memory allocation error");
    else{
        characterize_adc1_ch_0();
    }
}

/*
    === NOTES FOR FFT ===

# New duration of delay:
    Since duration gives the information on how many ticks are needed to wait for sampling with a certain frequency,
    the sampling frequency to use for a more accurate approximation of the signal have to be 2 * f_max.
    The new duration to pass to the adc function for sampling the signal have to be duration = 1 / f_sampling =
    = 1 / (2 * f_max) = 1 / (2 * idx * n / (n * duration)) = 1 / (2 * idx / duration) = duration / (2 * idx)

# Trashold value:
    Obtaining a relevant value for the calculation of the max frequency to use for finding the new sampling frequqncy
    can be achieved through the z-score; calculating the z-score of a value tells if it is an outlier or not.

    z = (value - mean) / std_dev --> if z > threshold so value is an outlier

    Theshold value is defined as the mean value plus/minus 3 scale order of standard deviation as base point (statistical approach).
*/
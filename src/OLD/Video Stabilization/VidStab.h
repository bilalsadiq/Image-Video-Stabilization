#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/video.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videostab.hpp"
#include "opencv2/opencv_modules.hpp"

#define arg(name) cmd.get<string>(name)
#define argb(name) cmd.get<bool>(name)
#define argi(name) cmd.get<int>(name)
#define argf(name) cmd.get<float>(name)
#define argd(name) cmd.get<double>(name)

using namespace std;
using namespace cv;
using namespace cv::videostab;


string saveMotionsPath;
double outputFps;
string outputPath;
bool quietMode;
Ptr<IFrameSource> stabilizedFrames;
MotionModel motionModel(const string &str);


void run()
{
	VideoWriter writer;
	Mat stabilizedFrame;
	int nframes = 0;

	// for each stabilized frame
	while (!(stabilizedFrame = stabilizedFrames->nextFrame()).empty())
	{
		nframes++;

		// init writer (once) and save stabilized frame
		if (!outputPath.empty())
		{
			if (!writer.isOpened())
				writer.open(outputPath, VideoWriter::fourcc('X', 'V', 'I', 'D'),
					outputFps, stabilizedFrame.size());
			writer << stabilizedFrame;
		}

		// show stabilized frame
		if (!quietMode)
		{
			imshow("stabilizedFrame", stabilizedFrame);
			char key = static_cast<char>(waitKey(3));
			if (key == 27) { cout << endl; break; }
		}
	}

	cout << "processed frames: " << nframes << endl
		<< "finished\n";
}

void printHelp()
{
	cout << "OpenCV video stabilizer.\n"
		"Usage: videostab <file_path> [arguments]\n\n"
		"Arguments:\n"
		"  -m=, --model=(transl|transl_and_scale|rigid|similarity|affine|homography)\n"
		"      Set motion model. The default is affine.\n"
		"  -lp=, --lin-prog-motion-est=(yes|no)\n"
		"      Turn on/off LP based motion estimation. The default is no.\n"
		"  --subset=(<int_number>|auto)\n"
		"      Number of random samples per one motion hypothesis. The default is auto.\n"
		"  --thresh=(<float_number>|auto)\n"
		"      Maximum error to classify match as inlier. The default is auto.\n"
		"  --outlier-ratio=<float_number>\n"
		"      Motion estimation outlier ratio hypothesis. The default is 0.5.\n"
		"  --min-inlier-ratio=<float_number>\n"
		"      Minimum inlier ratio to decide if estimated motion is OK. The default is 0.1.\n"
		"  --nkps=<int_number>\n"
		"      Number of keypoints to find in each frame. The default is 1000.\n"
		"  --local-outlier-rejection=(yes|no)\n"
		"      Perform local outlier rejection. The default is no.\n\n"
		"  -sm=, --save-motions=(<file_path>|no)\n"
		"      Save estimated motions into file. The default is no.\n"
		"  -lm=, --load-motions=(<file_path>|no)\n"
		"      Load motions from file. The default is no.\n\n"
		"  -r=, --radius=<int_number>\n"
		"      Set sliding window radius. The default is 15.\n"
		"  --stdev=(<float_number>|auto)\n"
		"      Set smoothing weights standard deviation. The default is auto\n"
		"      (i.e. sqrt(radius)).\n"
		"  -lps=, --lin-prog-stab=(yes|no)\n"
		"      Turn on/off linear programming based stabilization method.\n"
		"  --lps-trim-ratio=(<float_number>|auto)\n"
		"      Trimming ratio used in linear programming based method.\n"
		"  --lps-w1=(<float_number>|1)\n"
		"      1st derivative weight. The default is 1.\n"
		"  --lps-w2=(<float_number>|10)\n"
		"      2nd derivative weight. The default is 10.\n"
		"  --lps-w3=(<float_number>|100)\n"
		"      3rd derivative weight. The default is 100.\n"
		"  --lps-w4=(<float_number>|100)\n"
		"      Non-translation motion components weight. The default is 100.\n\n"
		"  --deblur=(yes|no)\n"
		"      Do deblurring.\n"
		"  --deblur-sens=<float_number>\n"
		"      Set deblurring sensitivity (from 0 to +inf). The default is 0.1.\n\n"
		"  -t=, --trim-ratio=<float_number>\n"
		"      Set trimming ratio (from 0 to 0.5). The default is 0.1.\n"
		"  -et=, --est-trim=(yes|no)\n"
		"      Estimate trim ratio automatically. The default is yes.\n"
		"  -ic=, --incl-constr=(yes|no)\n"
		"      Ensure the inclusion constraint is always satisfied. The default is no.\n\n"
		"  -bm=, --border-mode=(replicate|reflect|const)\n"
		"      Set border extrapolation mode. The default is replicate.\n\n"
		"  --mosaic=(yes|no)\n"
		"      Do consistent mosaicing. The default is no.\n"
		"  --mosaic-stdev=<float_number>\n"
		"      Consistent mosaicing stdev threshold. The default is 10.0.\n\n"
		"  -mi=, --motion-inpaint=(yes|no)\n"
		"      Do motion inpainting (requires CUDA support). The default is no.\n"
		"  --mi-dist-thresh=<float_number>\n"
		"      Estimated flow distance threshold for motion inpainting. The default is 5.0.\n\n"
		"  -ci=, --color-inpaint=(no|average|ns|telea)\n"
		"      Do color inpainting. The defailt is no.\n"
		"  --ci-radius=<float_number>\n"
		"      Set color inpainting radius (for ns and telea options only).\n"
		"      The default is 2.0\n\n"
		"  -ws=, --wobble-suppress=(yes|no)\n"
		"      Perform wobble suppression. The default is no.\n"
		"  --ws-lp=(yes|no)\n"
		"      Turn on/off LP based motion estimation. The default is no.\n"
		"  --ws-period=<int_number>\n"
		"      Set wobble suppression period. The default is 30.\n"
		"  --ws-model=(transl|transl_and_scale|rigid|similarity|affine|homography)\n"
		"      Set wobble suppression motion model (must have more DOF than motion \n"
		"      estimation model). The default is homography.\n"
		"  --ws-subset=(<int_number>|auto)\n"
		"      Number of random samples per one motion hypothesis. The default is auto.\n"
		"  --ws-thresh=(<float_number>|auto)\n"
		"      Maximum error to classify match as inlier. The default is auto.\n"
		"  --ws-outlier-ratio=<float_number>\n"
		"      Motion estimation outlier ratio hypothesis. The default is 0.5.\n"
		"  --ws-min-inlier-ratio=<float_number>\n"
		"      Minimum inlier ratio to decide if estimated motion is OK. The default is 0.1.\n"
		"  --ws-nkps=<int_number>\n"
		"      Number of keypoints to find in each frame. The default is 1000.\n"
		"  --ws-local-outlier-rejection=(yes|no)\n"
		"      Perform local outlier rejection. The default is no.\n\n"
		"  -sm2=, --save-motions2=(<file_path>|no)\n"
		"      Save motions estimated for wobble suppression. The default is no.\n"
		"  -lm2=, --load-motions2=(<file_path>|no)\n"
		"      Load motions for wobble suppression from file. The default is no.\n\n"
		"  -gpu=(yes|no)\n"
		"      Use CUDA optimization whenever possible. The default is no.\n\n"
		"  -o=, --output=(no|<file_path>)\n"
		"      Set output file path explicitely. The default is stabilized.avi.\n"
		"  --fps=(<float_number>|auto)\n"
		"      Set output video FPS explicitely. By default the source FPS is used (auto).\n"
		"  -q, --quiet\n"
		"      Don't show output video frames.\n\n"
		"  -h, --help\n"
		"      Print help.\n\n"
		"Note: some argument configurations lead to two passes, some to single pass.\n\n";
}


MotionModel motionModel(const string &str)
{
	if (str == "transl")
		return MM_TRANSLATION;
	if (str == "transl_and_scale")
		return MM_TRANSLATION_AND_SCALE;
	if (str == "rigid")
		return MM_RIGID;
	if (str == "similarity")
		return MM_SIMILARITY;
	if (str == "affine")
		return MM_AFFINE;
	if (str == "homography")
		return MM_HOMOGRAPHY;
	throw runtime_error("unknown motion model: " + str);
}

class IMotionEstimatorBuilder
{
public:
	virtual ~IMotionEstimatorBuilder() {}
	virtual Ptr<ImageMotionEstimatorBase> build() = 0;
protected:
	IMotionEstimatorBuilder(CommandLineParser &command) : cmd(command) {}
	CommandLineParser cmd;
};


class MotionEstimatorRansacL2Builder : public IMotionEstimatorBuilder
{
public:
	MotionEstimatorRansacL2Builder(CommandLineParser &command, bool use_gpu, const string &_prefix = "")
		: IMotionEstimatorBuilder(command), gpu(use_gpu), prefix(_prefix) {}

	virtual Ptr<ImageMotionEstimatorBase> build()
	{
		Ptr<MotionEstimatorRansacL2> est = makePtr<MotionEstimatorRansacL2>(motionModel(arg(prefix + "model")));

		RansacParams ransac = est->ransacParams();
		if (arg(prefix + "subset") != "auto")
			ransac.size = argi(prefix + "subset");
		if (arg(prefix + "thresh") != "auto")
			ransac.thresh = argf(prefix + "thresh");
		ransac.eps = argf(prefix + "outlier-ratio");
		est->setRansacParams(ransac);

		est->setMinInlierRatio(argf(prefix + "min-inlier-ratio"));

		Ptr<IOutlierRejector> outlierRejector = makePtr<NullOutlierRejector>();
		if (arg(prefix + "local-outlier-rejection") == "yes")
		{
			Ptr<TranslationBasedLocalOutlierRejector> tblor = makePtr<TranslationBasedLocalOutlierRejector>();
			RansacParams ransacParams = tblor->ransacParams();
			if (arg(prefix + "thresh") != "auto")
				ransacParams.thresh = argf(prefix + "thresh");
			tblor->setRansacParams(ransacParams);
			outlierRejector = tblor;
		}

#if defined(HAVE_OPENCV_CUDAIMGPROC) && defined(HAVE_OPENCV_CUDAOPTFLOW)
		if (gpu)
		{
			Ptr<KeypointBasedMotionEstimatorGpu> kbest = makePtr<KeypointBasedMotionEstimatorGpu>(est);
			kbest->setOutlierRejector(outlierRejector);
			return kbest;
		}
#endif

		Ptr<KeypointBasedMotionEstimator> kbest = makePtr<KeypointBasedMotionEstimator>(est);
		kbest->setDetector(GFTTDetector::create(argi(prefix + "nkps")));
		kbest->setOutlierRejector(outlierRejector);
		return kbest;
	}
private:
	bool gpu;
	string prefix;
};


class MotionEstimatorL1Builder : public IMotionEstimatorBuilder
{
public:
	MotionEstimatorL1Builder(CommandLineParser &command, bool use_gpu, const string &_prefix = "")
		: IMotionEstimatorBuilder(command), gpu(use_gpu), prefix(_prefix) {}

	virtual Ptr<ImageMotionEstimatorBase> build()
	{
		Ptr<MotionEstimatorL1> est = makePtr<MotionEstimatorL1>(motionModel(arg(prefix + "model")));

		Ptr<IOutlierRejector> outlierRejector = makePtr<NullOutlierRejector>();
		if (arg(prefix + "local-outlier-rejection") == "yes")
		{
			Ptr<TranslationBasedLocalOutlierRejector> tblor = makePtr<TranslationBasedLocalOutlierRejector>();
			RansacParams ransacParams = tblor->ransacParams();
			if (arg(prefix + "thresh") != "auto")
				ransacParams.thresh = argf(prefix + "thresh");
			tblor->setRansacParams(ransacParams);
			outlierRejector = tblor;
		}

#if defined(HAVE_OPENCV_CUDAIMGPROC) && defined(HAVE_OPENCV_CUDAOPTFLOW)
		if (gpu)
		{
			Ptr<KeypointBasedMotionEstimatorGpu> kbest = makePtr<KeypointBasedMotionEstimatorGpu>(est);
			kbest->setOutlierRejector(outlierRejector);
			return kbest;
		}
#endif

		Ptr<KeypointBasedMotionEstimator> kbest = makePtr<KeypointBasedMotionEstimator>(est);
		kbest->setDetector(GFTTDetector::create(argi(prefix + "nkps")));
		kbest->setOutlierRejector(outlierRejector);
		return kbest;
	}
private:
	bool gpu;
	string prefix;
};

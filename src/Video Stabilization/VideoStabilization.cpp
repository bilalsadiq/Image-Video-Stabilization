/*
Added By Bilal and Sadia
Edited By Stephen
This program will take an input file(video file),
which will detect motion and Shaking of the video 
then this algorithm will run. 
After finish running, It  will stabilized the video
and will saved on the same directory as a .avi video file.
*/


#include "VidStab.h"

int main(int argc, const char **argv)
{
	try
	{
		const char *keys =
			"{ @1                       |           | }"
			"{ m  model                 | affine    | }"
			"{ lp lin-prog-motion-est   | no        | }"
			"{  subset                  | auto      | }"
			"{  thresh                  | auto | }"
			"{  outlier-ratio           | 0.5 | }"
			"{  min-inlier-ratio        | 0.1 | }"
			"{  nkps                    | 1000 | }"
			"{  extra-kps               | 0 | }"
			"{  local-outlier-rejection | no | }"
			"{ sm  save-motions         | no | }"
			"{ lm  load-motions         | no | }"
			"{ r  radius                | 15 | }"
			"{  stdev                   | auto | }"
			"{ lps  lin-prog-stab       | no | }"
			"{  lps-trim-ratio          | auto | }"
			"{  lps-w1                  | 1 | }"
			"{  lps-w2                  | 10 | }"
			"{  lps-w3                  | 100 | }"
			"{  lps-w4                  | 100 | }"
			"{  deblur                  | no | }"
			"{  deblur-sens             | 0.1 | }"
			"{ et  est-trim             | yes | }"
			"{ t  trim-ratio            | 0.1 | }"
			"{ ic  incl-constr          | no | }"
			"{ bm  border-mode          | replicate | }"
			"{  mosaic                  | no | }"
			"{ ms  mosaic-stdev         | 10.0 | }"
			"{ mi  motion-inpaint       | no | }"
			"{  mi-dist-thresh          | 5.0 | }"
			"{ ci color-inpaint         | no | }"
			"{  ci-radius               | 2 | }"
			"{ ws  wobble-suppress      | no | }"
			"{  ws-period               | 30 | }"
			"{  ws-model                | homography | }"
			"{  ws-subset               | auto | }"
			"{  ws-thresh               | auto | }"
			"{  ws-outlier-ratio        | 0.5 | }"
			"{  ws-min-inlier-ratio     | 0.1 | }"
			"{  ws-nkps                 | 1000 | }"
			"{  ws-extra-kps            | 0 | }"
			"{  ws-local-outlier-rejection | no | }"
			"{  ws-lp                   | no | }"
			"{ sm2 save-motions2        | no | }"
			"{ lm2 load-motions2        | no | }"
			"{ gpu                      | no | }"
			"{ o  output                | stabilized.avi | }"
			"{ fps                      | auto | }"
			"{ q quiet                  |  | }"
			"{ h help                   |  | }";
		
		CommandLineParser cmd(argc, argv, keys);

		// parse command arguments

		if (argb("help"))
		{
			printHelp();
			return 0;
		}

		if (arg("gpu") == "yes")
		{
			cout << "initializing GPU..."; cout.flush();
			Mat hostTmp = Mat::zeros(1, 1, CV_32F);
			cuda::GpuMat deviceTmp;
			deviceTmp.upload(hostTmp);
			cout << endl;
		}

		StabilizerBase *stabilizer = 0;

		// check if source video is specified

		string inputPath = "Shaky_Footage.mp4";
		if (inputPath.empty())
			throw runtime_error("specify video file path");

		// get source video parameters

		Ptr<VideoFileSource> source = makePtr<VideoFileSource>(inputPath);
		cout << "frame count (rough): " << source->count() << endl;
		if (arg("fps") == "auto")
			outputFps = source->fps();
		else
			outputFps = argd("fps");

		// prepare motion estimation builders

		Ptr<IMotionEstimatorBuilder> motionEstBuilder;
		if (arg("lin-prog-motion-est") == "yes")
			motionEstBuilder.reset(new MotionEstimatorL1Builder(cmd, arg("gpu") == "yes"));
		else
			motionEstBuilder.reset(new MotionEstimatorRansacL2Builder(cmd, arg("gpu") == "yes"));

		Ptr<IMotionEstimatorBuilder> wsMotionEstBuilder;
		if (arg("ws-lp") == "yes")
			wsMotionEstBuilder.reset(new MotionEstimatorL1Builder(cmd, arg("gpu") == "yes", "ws-"));
		else
			wsMotionEstBuilder.reset(new MotionEstimatorRansacL2Builder(cmd, arg("gpu") == "yes", "ws-"));

		// determine whether we must use one pass or two pass stabilizer
		bool isTwoPass =
			arg("est-trim") == "yes" || arg("wobble-suppress") == "yes" || arg("lin-prog-stab") == "yes";

		if (isTwoPass)
		{
			// we must use two pass stabilizer

			TwoPassStabilizer *twoPassStabilizer = new TwoPassStabilizer();
			stabilizer = twoPassStabilizer;
			twoPassStabilizer->setEstimateTrimRatio(arg("est-trim") == "yes");

			// determine stabilization technique

			if (arg("lin-prog-stab") == "yes")
			{
				Ptr<LpMotionStabilizer> stab = makePtr<LpMotionStabilizer>();
				stab->setFrameSize(Size(source->width(), source->height()));
				stab->setTrimRatio(arg("lps-trim-ratio") == "auto" ? argf("trim-ratio") : argf("lps-trim-ratio"));
				stab->setWeight1(argf("lps-w1"));
				stab->setWeight2(argf("lps-w2"));
				stab->setWeight3(argf("lps-w3"));
				stab->setWeight4(argf("lps-w4"));
				twoPassStabilizer->setMotionStabilizer(stab);
			}
			else if (arg("stdev") == "auto")
				twoPassStabilizer->setMotionStabilizer(makePtr<GaussianMotionFilter>(argi("radius")));
			else
				twoPassStabilizer->setMotionStabilizer(makePtr<GaussianMotionFilter>(argi("radius"), argf("stdev")));

			// init wobble suppressor if necessary

			if (arg("wobble-suppress") == "yes")
			{
				Ptr<MoreAccurateMotionWobbleSuppressorBase> ws = makePtr<MoreAccurateMotionWobbleSuppressor>();
				if (arg("gpu") == "yes")
#ifdef HAVE_OPENCV_CUDAWARPING
					ws = makePtr<MoreAccurateMotionWobbleSuppressorGpu>();
#else
					throw runtime_error("OpenCV is built without CUDA support");
#endif

				ws->setMotionEstimator(wsMotionEstBuilder->build());
				ws->setPeriod(argi("ws-period"));
				twoPassStabilizer->setWobbleSuppressor(ws);

				MotionModel model = ws->motionEstimator()->motionModel();
				if (arg("load-motions2") != "no")
				{
					ws->setMotionEstimator(makePtr<FromFileMotionReader>(arg("load-motions2")));
					ws->motionEstimator()->setMotionModel(model);
				}
				if (arg("save-motions2") != "no")
				{
					ws->setMotionEstimator(makePtr<ToFileMotionWriter>(arg("save-motions2"), ws->motionEstimator()));
					ws->motionEstimator()->setMotionModel(model);
				}
			}
		}
		else
		{
			// we must use one pass stabilizer

			OnePassStabilizer *onePassStabilizer = new OnePassStabilizer();
			stabilizer = onePassStabilizer;
			if (arg("stdev") == "auto")
				onePassStabilizer->setMotionFilter(makePtr<GaussianMotionFilter>(argi("radius")));
			else
				onePassStabilizer->setMotionFilter(makePtr<GaussianMotionFilter>(argi("radius"), argf("stdev")));
		}

		stabilizer->setFrameSource(source);
		stabilizer->setMotionEstimator(motionEstBuilder->build());

		// cast stabilizer to simple frame source interface to read stabilized frames
		stabilizedFrames.reset(dynamic_cast<IFrameSource*>(stabilizer));

		MotionModel model = stabilizer->motionEstimator()->motionModel();
		if (arg("load-motions") != "no")
		{
			stabilizer->setMotionEstimator(makePtr<FromFileMotionReader>(arg("load-motions")));
			stabilizer->motionEstimator()->setMotionModel(model);
		}
		if (arg("save-motions") != "no")
		{
			stabilizer->setMotionEstimator(makePtr<ToFileMotionWriter>(arg("save-motions"), stabilizer->motionEstimator()));
			stabilizer->motionEstimator()->setMotionModel(model);
		}

		stabilizer->setRadius(argi("radius"));

		// init deblurer
		if (arg("deblur") == "yes")
		{
			Ptr<WeightingDeblurer> deblurer = makePtr<WeightingDeblurer>();
			deblurer->setRadius(argi("radius"));
			deblurer->setSensitivity(argf("deblur-sens"));
			stabilizer->setDeblurer(deblurer);
		}

		// set up trimming paramters
		stabilizer->setTrimRatio(argf("trim-ratio"));
		stabilizer->setCorrectionForInclusion(arg("incl-constr") == "yes");

		if (arg("border-mode") == "reflect")
			stabilizer->setBorderMode(BORDER_REFLECT);
		else if (arg("border-mode") == "replicate")
			stabilizer->setBorderMode(BORDER_REPLICATE);
		else if (arg("border-mode") == "const")
			stabilizer->setBorderMode(BORDER_CONSTANT);
		else
			throw runtime_error("unknown border extrapolation mode: "
				+ cmd.get<string>("border-mode"));

		// init inpainter
		InpaintingPipeline *inpainters = new InpaintingPipeline();
		Ptr<InpainterBase> inpainters_(inpainters);
		if (arg("mosaic") == "yes")
		{
			Ptr<ConsistentMosaicInpainter> inp = makePtr<ConsistentMosaicInpainter>();
			inp->setStdevThresh(argf("mosaic-stdev"));
			inpainters->pushBack(inp);
		}
		if (arg("motion-inpaint") == "yes")
		{
			Ptr<MotionInpainter> inp = makePtr<MotionInpainter>();
			inp->setDistThreshold(argf("mi-dist-thresh"));
			inpainters->pushBack(inp);
		}
		if (arg("color-inpaint") == "average")
			inpainters->pushBack(makePtr<ColorAverageInpainter>());
		else if (arg("color-inpaint") == "ns")
			inpainters->pushBack(makePtr<ColorInpainter>(int(INPAINT_NS), argd("ci-radius")));
		else if (arg("color-inpaint") == "telea")
			inpainters->pushBack(makePtr<ColorInpainter>(int(INPAINT_TELEA), argd("ci-radius")));
		else if (arg("color-inpaint") != "no")
			throw runtime_error("unknown color inpainting method: " + arg("color-inpaint"));
		if (!inpainters->empty())
		{
			inpainters->setRadius(argi("radius"));
			stabilizer->setInpainter(inpainters_);
		}

		if (arg("output") != "no")
			outputPath = arg("output");

		quietMode = argb("quiet");

		run();
	}
	catch (const exception &e)
	{
		cout << "error: " << e.what() << endl;
		stabilizedFrames.release();
		return -1;
	}
	stabilizedFrames.release();
	return 0;
}


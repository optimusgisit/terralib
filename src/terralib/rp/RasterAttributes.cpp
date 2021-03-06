/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file terralib/rp/RasterAttributes.cpp

  \brief Extraction of attributes from Raster, Bands, and Polygons.
*/

// TerraLib
#include "../common/progress/TaskProgress.h"
#include "../geometry.h"
#include "../raster/Band.h"
#include "../raster/BandIterator.h"
#include "../raster/Grid.h"
#include "../raster/PositionIterator.h"
#include "../raster/Raster.h"
#include "../raster/Utils.h"
#include "../statistics.h"
#include "RasterAttributes.h"

te::rp::RasterAttributes::RasterAttributes()
{
}

te::rp::RasterAttributes::~RasterAttributes()
{
}

bool te::rp::RasterAttributes::initialize(const AlgorithmInputParameters& inputParams) throw(te::rp::Exception)
{
  return true;
}

bool te::rp::RasterAttributes::isInitialized() const
{
  return true;
}

bool te::rp::RasterAttributes::execute(AlgorithmOutputParameters& outputParams) throw(te::rp::Exception)
{
  return true;
}

void te::rp::RasterAttributes::reset() throw(te::rp::Exception)
{
}

std::vector<std::complex<double> > te::rp::RasterAttributes::getComplexValuesFromBand(const te::rst::Raster& raster, unsigned int band, const te::gm::Polygon& polygon)
{
  std::vector<double> dvalues = getValuesFromBand(raster, band, polygon);
  std::vector<std::complex<double> > values;

  for (unsigned int i = 0; i < dvalues.size(); i++)
    values.push_back(dvalues[i]);

  return values;
}

std::vector<double> te::rp::RasterAttributes::getValuesFromBand(const te::rst::Raster& raster, unsigned int band, const te::gm::Polygon& polygon)
{
  assert(band < raster.getNumberOfBands());

  std::vector<double> values;

// create iterators for band and polygon
  te::rst::PolygonIterator<double> it = te::rst::PolygonIterator<double>::begin(&raster, &polygon);
  te::rst::PolygonIterator<double> itend = te::rst::PolygonIterator<double>::end(&raster, &polygon);

  while (it != itend)
  {
// using iterator
    values.push_back((*it)[band]);

    ++it;
  }

  return values;
}

std::vector<std::vector<std::complex<double> > > te::rp::RasterAttributes::getComplexValuesFromRaster(const te::rst::Raster& raster, const te::gm::Polygon& polygon, std::vector<unsigned int> bands,
																									  unsigned int rowstep, unsigned int colstep)
{
  assert(bands.size() > 0);
  assert(bands.size() <= raster.getNumberOfBands());

  assert(rowstep > 0);
  assert(colstep > 0);

  std::vector<std::vector<std::complex<double> > > allvalues;
  std::complex<double> value;

// create iterators for band and polygon
  te::rst::PolygonIterator<double> it = te::rst::PolygonIterator<double>::begin(&raster, &polygon);
  te::rst::PolygonIterator<double> itend = te::rst::PolygonIterator<double>::end(&raster, &polygon);

  //unsigned int initrow = it.getRow();

  for (unsigned int i = 0; i < bands.size(); i++)
    allvalues.push_back(std::vector<std::complex< double > > ());
  
  while (it != itend)
  {
    for (unsigned int i = 0; i < bands.size(); i++)
    {
      raster.getValue(it.getColumn(), it.getRow(), value, bands[i]);
      allvalues[i].push_back(value);
    }

    ++it;
  }

  return allvalues;
	
	
	/*  std::vector<std::vector<double> > dallvalues = getValuesFromRaster(raster, polygon, bands);
  std::vector<std::complex<double> > values;
  std::vector<std::vector<std::complex<double> > > allvalues;

  for (unsigned int i = 0; i < dallvalues.size(); i++)
  {
    values.clear();
    for (unsigned int j = 0; j < dallvalues[i].size(); j++)
      values.push_back(dallvalues[i][j]);
    allvalues.push_back(values);
  }

  return allvalues;*/
}

std::vector<std::vector<double> > te::rp::RasterAttributes::getValuesFromRaster(const te::rst::Raster& raster, const te::gm::Polygon& polygon, std::vector<unsigned int> bands,
	                                                                            unsigned int rowstep, unsigned int colstep)
{
  assert(bands.size() > 0);
  assert(bands.size() <= raster.getNumberOfBands());

  assert(rowstep > 0);
  assert(colstep > 0);

  std::vector<std::vector<double> > allvalues;
  double value;

// create iterators for band and polygon
  te::rst::PolygonIterator<double> it = te::rst::PolygonIterator<double>::begin(&raster, &polygon);
  te::rst::PolygonIterator<double> itend = te::rst::PolygonIterator<double>::end(&raster, &polygon);

  unsigned int initrow = it.getRow(), initcol;

  for (unsigned int i = 0; i < bands.size(); i++)
    allvalues.push_back(std::vector<double> ());
  while (it != itend)
  {
	  if(!((it.getRow() - initrow) % rowstep))
	  {
		for (unsigned int i = 0; i < bands.size(); i++)
		{
		  raster.getValue(it.getColumn(), it.getRow(), value, bands[i]);
		  allvalues[i].push_back(value);
		}
	  }

	  unsigned posstep = it.getColumn() + colstep;

	  initcol = it.getColumn();

	  for(initcol; initcol < posstep; initcol++)
		++it;
	  
  }

  return allvalues;
}

te::stat::NumericStatisticalSummary te::rp::RasterAttributes::getStatistics(std::vector<double>& pixels)
{
  assert(pixels.size() > 0);

  te::stat::NumericStatisticalSummary summary;
  te::stat::GetNumericStatisticalSummary(pixels, summary);
  te::stat::Mode(pixels, summary);

  return summary;
}

void te::rp::RasterAttributes::getStatisticsFromPolygon(const te::rst::Raster& raster, unsigned int band, const te::gm::Polygon& polygon, te::stat::NumericStatisticalSummary &summary) {
    assert(band < raster.getNumberOfBands());

    te::rst::PolygonIterator<double> it = te::rst::PolygonIterator<double>::begin(&raster, &polygon);
    te::rst::PolygonIterator<double> itEnd = te::rst::PolygonIterator<double>::end(&raster, &polygon);

    std::vector<double> values;
    std::map<double, int> modeMap;

    double delta = 0.0;
    double delta_n = 0.0;
    double delta_n2 = 0.0;
    double term1 = 0.0;
    double mean = 0.0;
    double M2 = 0.0;
    double M3 = 0.0;
    double M4 = 0.0;

    while (it != itEnd) {
        double value = (*it)[band];

        if (value != raster.getBand(band)->getProperty()->m_noDataValue) {
            ++summary.m_validCount;

            values.push_back(value);
            summary.m_sum += value;

            ++modeMap[value];

            // Computing summs of differences for the mean (used to variance, std deviation, skewness and kurtosis)
            delta = value - mean;
            delta_n = delta / summary.m_validCount;
            delta_n2 = delta_n * delta_n;
            term1 = delta * delta_n * (summary.m_validCount - 1);
            mean += delta_n;
            M4 += term1 * delta_n2 * (summary.m_validCount * summary.m_validCount - 3 * summary.m_validCount + 3) + 6 * delta_n2 * M2 - 4 * delta_n * M3;
            M3 += term1 * delta_n * (summary.m_validCount - 2) - 3 * delta_n * M2;
            M2 += term1;
        }
        ++summary.m_count;

        ++it;
    }

    summary.m_mean = summary.m_sum / summary.m_validCount;

    if (summary.m_validCount > 3) {
        summary.m_variance = M2 / (summary.m_validCount - 1.0);
        summary.m_stdDeviation = sqrt(summary.m_variance);

        summary.m_skewness = M3 / pow(M2, 1.5);
        summary.m_skewness *= (summary.m_validCount * sqrt(summary.m_validCount - 1)) / (summary.m_validCount - 2);

        summary.m_kurtosis = M4 / (M2 * M2);
        summary.m_kurtosis *= ((summary.m_validCount * (summary.m_validCount + 1) * (summary.m_validCount - 1)) / ((summary.m_validCount - 2) * (summary.m_validCount - 3)));
    } else if (summary.m_validCount > 2){
        summary.m_variance = M2 / (summary.m_validCount - 1.0);
        summary.m_stdDeviation = sqrt(summary.m_variance);

        summary.m_skewness = M3 / pow(M2, 1.5);
        summary.m_skewness *= (summary.m_validCount * sqrt(summary.m_validCount - 1)) / (summary.m_validCount - 2);

        summary.m_kurtosis = 0;
    } else if (summary.m_validCount > 1) {
        summary.m_variance = M2 / (summary.m_validCount - 1.0);
        summary.m_stdDeviation = sqrt(summary.m_variance);

        summary.m_skewness = 0.0;
        summary.m_kurtosis = 0.0;
    } else {
        summary.m_variance = 0.0;
        summary.m_stdDeviation = 0.0;
        summary.m_skewness = 0.0;
        summary.m_kurtosis = 0.0;
    }

    summary.m_varCoeff = (100 * summary.m_stdDeviation) / summary.m_mean;

    std::sort(values.begin(), values.end());

    summary.m_minVal = *values.begin();
    summary.m_maxVal = values[summary.m_validCount - 1];
    summary.m_amplitude = summary.m_maxVal - summary.m_minVal;

    if ((summary.m_validCount % 2) == 0)
        summary.m_median = (values[(summary.m_validCount / 2)] + values[(summary.m_validCount / 2 - 1)]) / 2.0;
    else
        summary.m_median = values[(summary.m_validCount - 1) / 2];

    std::map<double, int>::iterator itMode = modeMap.begin();
    int repeat = 0;

    while (itMode != modeMap.end()) {
        if (repeat < itMode->second) {
            repeat = itMode->second;
            summary.m_mode.clear();
            summary.m_mode.push_back(itMode->first);
        }
        else if (repeat == itMode->second) {
            summary.m_mode.push_back(itMode->first);
        }
        itMode++;
    }
}

te::stat::NumericStatisticalComplexSummary te::rp::RasterAttributes::getComplexStatistics(std::vector<std::complex < double > >& pixels)
{
  assert(pixels.size() > 0);

  te::stat::NumericStatisticalComplexSummary summary;
  te::stat::GetNumericComplexStatisticalSummary(pixels, summary);

  return summary;
}

boost::numeric::ublas::matrix<std::complex < double> > te::rp::RasterAttributes::getComplexCovarianceMatrix(const std::vector<std::vector<std::complex < double > > >& vpixels, const std::vector<std::complex < double > >& vmeans)
{
	  for (unsigned int i = 0; i < vpixels.size(); i++)
		assert(vpixels[i].size() > 0);
	  for (unsigned int i = 1; i < vpixels.size(); i++)
		assert(vpixels[0].size() == vpixels[i].size());
	  assert(vpixels.size() == vmeans.size());

	  unsigned int i;
	  unsigned int j;
	  unsigned int k;
	  unsigned int nbands = vpixels.size();
	  unsigned int nvalues = vpixels[0].size();

	  boost::numeric::ublas::matrix<std::complex < double > > covariance(nbands, nbands);

	// with few values, the covariance is default
	  if (nvalues < 2)
	  {
		for (i = 0; i < nbands; i++)
		{
		  for (j = 0; j < nbands; j++)
			covariance(i, j) = 0.0;
		  covariance(i, i) = 1000.0;
		}

		return covariance;
	  }

	// compute covariance matrix based on values and means
	  te::common::TaskProgress taskProgress("Computing covariance complex matrix", te::common::TaskProgress::UNDEFINED, nbands * nbands);
	  std::complex<double> sum;
	  for (i = 0; i < nbands; i++)
		for (j = 0; j < nbands; j++)
		{
		  taskProgress.pulse();
      
		  sum = std::complex<double> (0.0, 0.0);

		  for (k = 0; k < nvalues; k++)
			sum += (vpixels[i][k] - vmeans[i]) * (vpixels[j][k] - vmeans[j]);

		  covariance(i, j) = sum / (nvalues - 1.0);
		}

	  return covariance;

}

boost::numeric::ublas::matrix<double> te::rp::RasterAttributes::getCovarianceMatrix(const std::vector<std::vector<double> >& vpixels, const std::vector<double>& vmeans)
{
  for (unsigned int i = 0; i < vpixels.size(); i++)
    assert(vpixels[i].size() > 0);
  for (unsigned int i = 1; i < vpixels.size(); i++)
    assert(vpixels[0].size() == vpixels[i].size());
  assert(vpixels.size() == vmeans.size());

  unsigned int i;
  unsigned int j;
  unsigned int k;
  unsigned int nbands = vpixels.size();
  unsigned int nvalues = vpixels[0].size();

  boost::numeric::ublas::matrix<double> covariance(nbands, nbands);

// with few values, the covariance is default
  if (nvalues < 2)
  {
    for (i = 0; i < nbands; i++)
    {
      for (j = 0; j < nbands; j++)
        covariance(i, j) = 0.0;
      covariance(i, i) = 1000.0;
    }

    return covariance;
  }

// compute covariance matrix based on values and means
  te::common::TaskProgress taskProgress("Computing covariance matrix", te::common::TaskProgress::UNDEFINED, nbands * nbands);
  std::complex<double> sum;
  for (i = 0; i < nbands; i++)
    for (j = 0; j < nbands; j++)
    {
      taskProgress.pulse();
      
      sum = std::complex<double> (0.0, 0.0);

      for (k = 0; k < nvalues; k++)
        sum += (vpixels[i][k] - vmeans[i]) * (vpixels[j][k] - vmeans[j]);

      covariance(i, j) = sum.real() / (nvalues - 1);
    }

  return covariance;
}

boost::numeric::ublas::matrix<double> te::rp::RasterAttributes::getGLCM(const te::rst::Raster& rin, unsigned int band, int dx, int dy, 
    double minPixel, double maxPixel, double gLevels)
{
  assert(band < rin.getNumberOfBands());

  if (maxPixel == 0 && minPixel == 0) {
      throw te::common::Exception(TE_TR("GLCM can not be computed. Minimum and Maximum values are invalid."));
  }
  
  double minValueNormalization, maxValueNormalization, matrixLimit;
  bool normalize = false;
  
  if ((maxPixel - minPixel) > (gLevels - 1)) {
    minValueNormalization = 0;
    maxValueNormalization = gLevels - 1;
    matrixLimit = gLevels;
    normalize = true;
  } else {
    matrixLimit = (maxPixel - minPixel) + 1;
  }
  

  boost::numeric::ublas::matrix<double> glcm(matrixLimit, matrixLimit);
  glcm.clear();
  double pixel;
  double neighborPixel;
  double noDataValue = rin.getBand(band)->getProperty()->m_noDataValue;
  double N = 0.0;
  
// defining limits for iteration  
  int row_start = 0;
  int row_end = rin.getNumberOfRows();
  int column_start = 0;
  int column_end = rin.getNumberOfColumns();
  
  if (dy > 0)
    row_end -= dy;
  else
    row_start -= dy;

  if (dx > 0)
    column_end -= dx;
  else
    column_start -= dx;
  
// computing GLCM
  te::common::TaskProgress taskProgress("Computing the GLCM", te::common::TaskProgress::UNDEFINED, (row_end - row_start) * (column_end - column_start));
  for (int r = row_start; r < row_end; r++)
  {
    for (int c = column_start; c < column_end; c++)
    {
      taskProgress.pulse();
      
// get central pixel      
      rin.getValue(c, r, pixel, band);
      if (pixel == noDataValue)
        continue;

      double auxPixel = pixel;
      // normalizing between 0-maxValue
      if (normalize) {
          pixel = std::round((((maxValueNormalization - minValueNormalization) / (maxPixel - minPixel)) * (auxPixel - minPixel)) + minValueNormalization);
          if (pixel < 0)
              pixel = 0;
          else if (pixel > maxValueNormalization)
              pixel = maxValueNormalization;
      }
      
// get neighbor pixel
      rin.getValue(c + dx, r + dy, neighborPixel, band);
      if (neighborPixel == noDataValue)
        continue;

      double auxNeighbor = neighborPixel;
      // normalizing between 0-maxValue
      if (normalize) {
          neighborPixel = std::round((((maxValueNormalization - minValueNormalization) / (maxPixel - minPixel)) * (auxNeighbor - minPixel)) + minValueNormalization);
          if (neighborPixel < 0)
              neighborPixel = 0;
          else if (neighborPixel > maxValueNormalization)
              neighborPixel = maxValueNormalization;
      }

// update GLCM matrix
      glcm(pixel, neighborPixel) = glcm(pixel, neighborPixel) + 1;
      N++;
    }
  }

  if (N > 0.0)
  {
    for (unsigned int i = 0; i < glcm.size1(); i++)
      for (unsigned int j = 0; j < glcm.size2(); j++)
        glcm(i, j) = glcm(i, j) / N;
  }
  
  return glcm;
}
        
boost::numeric::ublas::matrix<double> te::rp::RasterAttributes::getGLCM(const te::rst::Raster& rin, unsigned int band, int dx, int dy, 
    const te::gm::Polygon& polygon, double minPixel, double maxPixel, double gLevels)
{
  if (maxPixel == 0 && minPixel == 0) {
    throw te::common::Exception(TE_TR("GLCM can not be computed. Minimum and Maximum values are invalid."));
  }

  // create raster crop with polygon
  std::map<std::string, std::string> rcropinfo;
  rcropinfo["FORCE_MEM_DRIVER"] = "TRUE";
  te::rst::RasterPtr rcrop = te::rst::CropRaster(rin, polygon, rcropinfo, "MEM");
  
  // call previous method using crop
  return getGLCM(*rcrop.get(), band, dx, dy, minPixel, maxPixel, gLevels);
}
        
te::rp::Texture te::rp::RasterAttributes::getGLCMMetrics(boost::numeric::ublas::matrix<double> glcm)
{
  te::rp::Texture metrics;
  
  unsigned int i;
  unsigned int j;
  double di;
  double dj;
  double di_minus_dj;
  double square_di_minus_dj;
  te::common::TaskProgress taskProgress("Computing GLCM Metrics", te::common::TaskProgress::UNDEFINED, glcm.size1() * glcm.size2());
  for (i = 0, di = 0.0; i < glcm.size1(); i++, di++)
  {
    for (j = 0, dj = 0.0; j < glcm.size2(); j++, dj++)
    {
      taskProgress.pulse();
      
      di_minus_dj = (di - dj);
      square_di_minus_dj = di_minus_dj * di_minus_dj;
      
      metrics.m_contrast += glcm(i, j) * square_di_minus_dj;
      metrics.m_dissimilarity += glcm(i, j) * std::abs(di_minus_dj);
      metrics.m_energy += glcm(i, j) * glcm(i, j);
      if (glcm(i, j) > 0)
        metrics.m_entropy += glcm(i, j) * (-1.0 * log(glcm(i, j)));
      metrics.m_homogeneity += glcm(i, j) / (1 + square_di_minus_dj);
    }
  }

  if (metrics.m_energy > 0)
    metrics.m_energy = std::sqrt(metrics.m_energy);

  return metrics;
}